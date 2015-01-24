/*Andrija Jovanov
  Presently nameless Javascript game engine for HTML5 games*/

//Keycode constants
LEFT = 37;
RIGHT = 39;
UP = 38;
DOWN = 40;
ENTER = 13;

//Class for the game world and its contents
function World(width, height, tileWidth){

    //Creates the canvas
	this.canvas = document.createElement("canvas");
	document.body.appendChild(this.canvas);
	this.context = this.canvas.getContext("2d");

	//Sets the canvas size
	this.width = width;
    this.height = height;
    this.canvas.width = this.width;
    this.canvas.height = this.height;

    /*Sets the world matrix
      This matrix is used for various calculations*/
    this.tileWidth = tileWidth;
    this.grid = new Array(this.width / this.tileWidth);
    for(i = 0; i < this.width / this.tileWidth; i++){

    	this.grid[i] = new Array(this.height / this.tileWidth);
    	for(j = 0; j < this.height / this.tileWidth; j++)
    		this.grid[i][j] = 0;

    }

	this.canvas.style.backgroundColor = "cyan";

}

//Starts up the main event loop
World.prototype.start = function(){

	//Sets the method "update" to run every 50 milliseconds
	this.updateTime = setInterval(update, 50);
	
	//Sets mouse functions
	document.onmousemove = this.setMousePos;
    document.onmousedown = this.mousePressedAction;
    document.onmouseup = this.mouseReleasedAction;
    document.onclick = this.mouseClickedAction;

}

//Stops the main event loop
World.prototype.stop = function(){

    //Stop having the function "update" run automatically
    clearInterval(this.updateTime);

}

//Tracks the mouse position
World.prototype.setMousePos = function(e){

	this.mouseX = e.pageX;
	this.mouseY = e.pageY;

}

World.prototype.getMouseXPos = function(){

	return document.mouseX;

}

World.prototype.getMouseYPos = function(){

	return document.mouseY;

}

World.prototype.mousePressedAction = function(e){

	this.mouseDown = true;

}

World.prototype.mouseReleasedAction = function(e){

	this.mouseDown = false;

}

World.prototype.mouseClickedAction = function(e){

	this.mouseClicked = true;

}

//Ensures that holding a mouse button down does not continuously trigger an action
World.prototype.isMouseClicked = function(){

	if(document.mouseClicked){

		document.mouseClicked = false;
		return true;

	}else
		return false;

}

//Gets the coordinates of the clicked sprite
World.prototype.getClickedSquare = function(){

	clickedSquare = new Array(2);
	clickedSquare[0] = Math.floor(document.mouseX / this.tileWidth);
	clickedSquare[1] = Math.floor(document.mouseY / this.tileWidth);

	return clickedSquare;

}

//Clears the canvas
World.prototype.erase = function(){

    this.context.clearRect(0, 0, this.width, this.height);

}

//Sets a sprite's position in the world matrix
World.prototype.setGridVal = function(x, y, id){

	this.grid[x][y] = id;

}

//Returns a sprite's identifier stored in the world matrix
World.prototype.getGridVal = function(x, y){

	return this.grid[x][y];

}



//Class for all characters and objects 
function Sprite(world, width, height, image, x, y, id){

	//Sets the sprite's initial values
	this.world = world;
	this.canvas = world.canvas;
	this.context = world.canvas.getContext("2d");
	this.width = width;
	this.height = height;
	this.idNum = id;

	this.image = new Image();
	this.image.src = image;

	this.x = x;
	this.y = y;
	this.xPos = (this.x * this.world.tileWidth) + (this.world.tileWidth / 2);
	this.yPos = (this.y * this.world.tileWidth) + (this.world.tileWidth / 2);
	
	//The following two variables are not needed for this game
	//this.relativeX = x;
	//this.relativeY = y;

	this.world.setGridVal(this.x, this.y, this.idNum);

	//Sets the connectors for this sprite (applies to tiles only, this will be changes in future versions)
	if(this.idNum == 1 || this.idNum == 3 || this.idNum == 4)
		this.topConnect = true;
	else
		this.topConnect = false;

	if(this.idNum == 1 || this.idNum == 5 || this.idNum == 6)
		this.bottomConnect = true;
	else
		this.bottomConnect = false;

	if(this.idNum == 2 || this.idNum == 3 || this.idNum == 5)
		this.leftConnect = true;
	else
		this.leftConnect = false;

	if(this.idNum == 2 || this.idNum == 4 || this.idNum == 6)
		this.rightConnect = true;
	else
		this.rightConnect = false;

}

Sprite.prototype.setX = function(x){

	this.x = x;
	this.xPos = (this.x * this.world.tileWidth) + (this.world.tileWidth / 2);

}

Sprite.prototype.setY = function(y){

	this.y = y;
	this.yPos = (this.y * this.world.tileWidth) + (this.world.tileWidth / 2);
	
}

Sprite.prototype.setId = function(id){

	this.idNum = id;

}

Sprite.prototype.setImage = function(image){

	this.image.src = image;

}

/*Makes a sprite's outline flash when it is clicked
  Note: originally it was supposed to change colour until cancelled by the following method,
  but that has not been correctly implemented yet*/
Sprite.prototype.setImageClicked = function(){

	this.context.lineWidth = 5;
	this.context.strokeStyle = "blue";
	this.context.strokeRect(this.xPos - (this.width / 2), this.yPos - (this.height / 2), this.height, this.width);

}

//Cancels the effects of the previous method
Sprite.prototype.setImageUnclicked = function(){

	this.context.clearRect(this.xPos - (this.width / 2), this.yPos - (this.height / 2), this.height, this.width);
	
}

Sprite.prototype.getX = function(){

	return this.x;

}

Sprite.prototype.getY = function(){

	return this.y;

}

Sprite.prototype.getId = function(){

	return this.idNum;

}

Sprite.prototype.getImage = function(){

	return this.image.src;

}

Sprite.prototype.setConnectors = function(top, bottom, left, right){

	this.topConnect = top;
	this.bottomConnect = bottom;
	this.leftConnect = left;
	this.rightConnect = right;
	
}

//Moves the sprite to a new location
Sprite.prototype.changePosition = function(x, y){

	this.setX(x);
	this.setY(y);

	this.world.setGridVal(this.x, this.y, this.idNum);

}

//Changes the sprite to resemble another sprite of the same kind
Sprite.prototype.transform = function(id, image){

	this.setId(id);
	this.setImage(image);

	this.world.setGridVal(this.x, this.y, this.idNum);

}

//Swaps the position of two sprites
Sprite.prototype.swap = function(swapSprite){

	var tempX = this.x;
	var tempY = this.y;

	this.changePosition(swapSprite.getX(), swapSprite.getY());
	swapSprite.changePosition(tempX, tempY);

}

//Redraws the sprite
Sprite.prototype.update = function(){

    this.context.save();

	this.context.translate(this.xPos, this.yPos);
	this.context.drawImage(this.image, 0 - (this.width / 2) + 1, 0 - (this.height / 2) + 1, this.width - 1, this.height - 1);

	this.context.restore();

}



//Timer class
function Timer(startTime, endTime, timeDiv){

	this.currentTime = startTime;
	this.endTime = endTime;
	self = this;

}

//Starts the timer's event loop
Timer.prototype.start = function(){

	//Set the timer to count down every second
	this.updateTime = setInterval(self.update, 1000);

}

//Stops the timer's event loop
Timer.prototype.stop = function(){

	//Stops the timer
    clearInterval(this.updateTime);

}

Timer.prototype.getCurrentTime = function(){

	return this.currentTime;

}

Timer.prototype.setCurrentTime = function(time){

	this.currentTime = time;

}

Timer.prototype.update = function(){

	self.currentTime--;

}