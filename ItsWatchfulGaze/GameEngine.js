/*Andrija Jovanov
  Presently nameless Javascript game engine for HTML5 games*/

//Keycode constants
LEFT = 37; UP = 38; RIGHT = 39; DOWN = 40;
ENTER = 13; SPACE = 32;

keyPressed = null;

//Class for the canvas and its contents
function World(width, height, canvasWidth, canvasHeight, tileWidth, colour){

    //Creates the canvas
	this.canvas = document.createElement("canvas");
	document.body.appendChild(this.canvas);
	this.context = this.canvas.getContext("2d");

	//Sets the canvas size
	this.width = width;
    this.height = height;
    this.canvas.width = canvasWidth;
    this.canvas.height = canvasHeight;

    /*Sets the world matrix
      This matrix is used for various calculations*/
    this.tileWidth = tileWidth;
    this.grid = new Array(this.width / this.tileWidth);
    for(i = 0; i < this.width / this.tileWidth; i++){

    	this.grid[i] = new Array(this.height / this.tileWidth);
    	for(j = 0; j < this.height / this.tileWidth; j++)
    		this.grid[i][j] = 0;

    }

	this.canvas.style.backgroundColor = colour;

}

//Starts up the canvas
World.prototype.start = function(){

	//Sets the method "update" to run every 50 milliseconds
	this.updateTime = setInterval(update, 50);
	
	//Sets mouse functions
	document.onmousemove = this.setMousePos;
	document.onmousedown = this.mousePressedAction;
	document.onmouseup = this.mouseReleasedAction;
	document.onclick = this.mouseClickedAction;

	//Sets the keyboard functions
	document.onkeydown = this.setKeyPressed;
	document.onkeyup = this.clearKeyPressed;

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

//Keeps track of the current key being pressed
World.prototype.setKeyPressed = function(e){

	keyPressed = e.keyCode;

}

World.prototype.clearKeyPressed = function(e){

	keyPressed = null;

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

	var i, j;

	//Sets the sprite's initial values
	this.world = world;
	this.canvas = world.canvas;
	this.context = world.canvas.getContext("2d");
	this.width = width;
	this.height = height;
	this.idNum = id;

	this.image = new Image();
	this.image.src = image;

	this.x = x;						//Position on the canvas
	this.y = y;
	this.relativeX = x;				//Position relative to the entire game world
	this.relativeY = y;
	this.formerX = this.relativeX;	//Previous relative position
	this.formerY = this.relativeY;

	//Stores sprite representation in the world's grid for certain sprites
	if(this.idNum > 0){

		if(this.world.tileWidth > 1){

			this.world.setGridVal(this.x, this.y, this.idNum);

		}else{

			for(i = this.relativeX; i < this.relativeX + this.width; i++)
				for(j = this.relativeY; j < this.relativeY + this.height; j++)
					this.world.setGridVal(i, j, this.idNum);

		}

	}

	this.visible = true;
	this.hasAnimation = false;

}

Sprite.prototype.setX = function(x){

	this.x = x;
	//this.xPos = (this.x * this.world.tileWidth) + (this.width / 2);

}

Sprite.prototype.setY = function(y){

	this.y = y;
	//this.yPos = (this.y * this.world.tileWidth) + (this.height / 2);
	
}

Sprite.prototype.moveX = function(dx){

	this.x += dx;
	this.relativeX += dx;
	//this.xPos = (this.x * this.world.tileWidth) + (this.width / 2);

}

Sprite.prototype.moveY = function(dy){

	this.y += dy;
	this.relativeY += dy;
	//this.yPos = (this.y * this.world.tileWidth) + (this.height / 2);
	
}

Sprite.prototype.setRelativeX = function(x){

	this.relativeX = x;

}

Sprite.prototype.setRelativeY = function(y){

	this.relativeY = y;
	
}

Sprite.prototype.setId = function(id){

	this.idNum = id;

}

Sprite.prototype.setImage = function(image){

	this.image.src = image;

}

Sprite.prototype.getX = function(){

	return this.x;

}

Sprite.prototype.getY = function(){

	return this.y;

}

Sprite.prototype.getRelativeX = function(){

	return this.relativeX;

}

Sprite.prototype.getRelativeY = function(){

	return this.relativeY;

}

Sprite.prototype.getId = function(){

	return this.idNum;

}

Sprite.prototype.getImage = function(){

	return this.image.src;

}

//Moves the sprite to a new location
Sprite.prototype.changePosition = function(x, y){

	this.setRelativeX(this.relativeX + (x - this.x));
	this.setRelativeY(this.relativeY + (y - this.y));
	this.setX(x);
	this.setY(y);

	if(this.idNum > 0){

		if(this.world.tileWidth > 1)
			this.world.setGridVal(this.relativeX, this.relativeY, this.idNum);
		else
			changeWorldPosition();

	}

}

//Changes the sprite's location in the world's grid
Sprite.prototype.changeWorldPosition = function(){

	for(i = this.formerX; i < this.formerX + this.width; i++)
		for(j = this.formerY; j < this.formerY + this.height; j++)
			this.world.setGridVal(i, j, 0);

	for(i = this.relativeX; i < this.relativeX + this.width; i++)
		for(j = this.relativeY; j < this.relativeY + this.height; j++)
			this.world.setGridVal(i, j, this.idNum);

	this.formerX = this.relativeX;
	this.formerY = this.relativeY;

}

//Changes the sprite to resemble another sprite of the same kind
Sprite.prototype.transform = function(id, image){

	this.setId(id);
	this.setImage(image);

	if(this.idNum > 0)
		this.world.setGridVal(this.x, this.y, this.idNum);

}

//Swaps the position of two sprites
Sprite.prototype.swap = function(swapSprite){

	var tempX = this.x;
	var tempY = this.y;

	this.changePosition(swapSprite.getX(), swapSprite.getY());
	swapSprite.changePosition(tempX, tempY);

}

//Moves the sprite in the opposite direction that the screen is scrolling in
Sprite.prototype.scrollPosition = function(x, y){

	//Position is position of center
	//Relative position to world does not change when scrolling
	this.setX(x);
	this.setY(y);

}

//Conceals the sprite so it is not drawn
Sprite.prototype.conceal = function(){

	this.visible = false;

}

//Reveals the sprite so it will be drawn
Sprite.prototype.reveal = function(){

	this.visible = true;

}

//Sets up the properties of a sprite's animations
Sprite.prototype.setAnimation = function(frameWidth, frameHeight, imageWidth, imageHeight, interval, animOrder){

	var i, j, k;

	this.frameWidth = frameWidth;
	this.frameHeight = frameHeight;
	this.animInterval = interval;
	this.animTimer = new Timer(0, 0, this.animInterval);

	this.rowFrames = imageWidth / this.frameWidth;
	this.colFrames = imageHeight / this.frameHeight;
	this.currentFrame = new Array(2);
	this.animFrames = new Array(this.rowFrames);
	this.animOrder = animOrder;
	this.animIndex = 0;

	for(i = 0; i < this.rowFrames; i++){

		this.animFrames[i] = new Array(this.colFrames);
		for(j = 0; j < this.colFrames; j++){

			this.animFrames[i][j] = new Array(2);
			this.animFrames[i][j][0] = frameWidth * i;
			this.animFrames[i][j][1] = frameHeight * j;

		}

	}

	this.currentFrame[0] = 0;
	this.currentFrame[1] = this.animOrder[this.animIndex];

	this.hasAnimation = true;

}

//Sets the row of the spritesheet to be used for drawing
Sprite.prototype.setCurrentRow = function(rowNum){

	this.currentFrame[0] = rowNum;

}

//Cycles through a spritesheet, drawing the frames to for an animation
Sprite.prototype.animDraw = function(){

	var xPos = (this.x * this.world.tileWidth) + (this.width / 2);
	var yPos = (this.y * this.world.tileWidth) + (this.height / 2);

	if(keyPressed != null && this.animTimer.getAnimTime() >= 1){

		this.animIndex = (this.animIndex + 1) % this.animOrder.length;
		this.currentFrame[1] = this.animOrder[this.animIndex];
		this.animTimer.setAnimTime(0);

	}

	this.context.translate(xPos, yPos);
	this.context.drawImage(this.image, this.currentFrame[1] * this.frameWidth, this.currentFrame[0] * this.frameHeight,
		                   this.frameWidth, this.frameHeight, 0 - (this.width / 2) + 1, 0 - (this.height / 2) + 1,
		                   this.width, this.height);

}

//Redraws the sprite
Sprite.prototype.draw = function(){

	var xPos = (this.x * this.world.tileWidth) + (this.width / 2);
	var yPos = (this.y * this.world.tileWidth) + (this.height / 2);

	this.context.translate(xPos, yPos);
	this.context.drawImage(this.image, 0 - (this.width / 2) + 1, 0 - (this.height / 2) + 1, this.width - 1, this.height - 1);

}

Sprite.prototype.update = function(){

	if(this.visible){

		this.context.save();

		if(this.hasAnimation)
			this.animDraw();
		else
			this.draw();

		this.context.restore();

	}

}



//Timer class
/*This class is presently set up with separate methods for different types of timers.
  This is because of an issue where distinct timer objects were sharing properties.
  This will be fixed in a later version.*/
function Timer(startTime, endTime, interval){

	this.currentTime = startTime;
	this.endTime = endTime;
	this.elapsedTime = 0;
	this.animTime = 0;
	this.interval = interval;
	self = this;

}

Timer.prototype.startCountUp = function(){

	//Set the timer to count up every "interval" milliseconds
	this.updateTime = setInterval(self.countUp, self.interval);

}

Timer.prototype.startCountDown = function(){

	//Set the timer to count down every "interval" milliseconds
	this.updateTime = setInterval(self.countDown, self.interval);

}

Timer.prototype.startAnimCount = function(){

	//Set the timer to count up every "interval" milliseconds
	this.updateTime = setInterval(self.animCount, self.interval);

}

Timer.prototype.stop = function(){

	//Stop the timer
	clearInterval(self.updateTime);

}

Timer.prototype.getCurrentTime = function(){

	return self.currentTime;

}

Timer.prototype.setCurrentTime = function(time){

	self.currentTime = time;

}

Timer.prototype.getElapsedTime = function(){

	return self.elapsedTime;

}

Timer.prototype.setElapsedTime = function(time){

	self.elapsedTime = time;

}

Timer.prototype.getAnimTime = function(){

	return self.animTime;

}

Timer.prototype.setAnimTime = function(time){

	self.animTime = time;

}

Timer.prototype.countUp = function(){

	self.elapsedTime++;

}

Timer.prototype.countDown = function(){

	self.currentTime--;

}

Timer.prototype.animCount = function(){

	self.animTime++;

}