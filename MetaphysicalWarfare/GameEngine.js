/*Andrija Jovanov
  Presently nameless Javascript game engine for HTML5 games*/

//Keycode constants
LEFT = 37; UP = 38; RIGHT = 39; DOWN = 40;
ENTER = 13; SPACE = 32;
A = 65; B = 66; C = 67; D = 68; E = 69; F = 70; G = 71;
H = 72; I = 73; J = 74; K = 75; L = 76; M = 77; N = 78;
O = 79; P = 80; Q = 81; R = 82; S = 83; T = 84; U = 85;
V = 86; W = 87; X = 88; Y = 89; Z = 90;
ZERO = 48; ONE = 49; TWO = 50; THREE = 51; FOUR = 52;
FIVE = 53; SIX = 54; SEVEN = 55; EIGHT = 56; NINE = 57;
STOP = 1;

keyPressed = new Array(256);

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
    		this.grid[i][j] = null;

    }

    this.gAccel = 5; //Acceleration due to gravity

	this.canvas.style.backgroundColor = colour;
	this.canvas.style.left = "0px";
    this.canvas.style.top = "0px";
    this.canvas.style.position = "absolute";

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

	keyPressed[e.keyCode] = true;

}

World.prototype.clearKeyPressed = function(e){

	keyPressed[e.keyCode] = false;

}

//Clears the canvas
World.prototype.erase = function(){

    this.context.clearRect(0, 0, this.width, this.height);

}

World.prototype.setGravity = function(gAccel){

	this.gAccel = gAccel;

}

World.prototype.getGravity = function(){

	return this.gAccel;

}

//Stores sprite representation in the world's grid for certain sprites
World.prototype.initGridSprite = function(sprite){

	if(sprite.idNum > 0){

		if(this.tileWidth > 1){

			this.setGridVal(sprite.x, sprite.y, sprite);

		}else{

			for(i = sprite.relativeX; i < sprite.relativeX + sprite.width; i++)
				for(j = sprite.relativeY; j < sprite.relativeY + sprite.height; j++)
					this.setGridVal(i, j, sprite);

		}

	}

}

//Sets a sprite's position in the world matrix
World.prototype.setGridVal = function(x, y, newVal){

	this.grid[x][y] = newVal;

}

//Returns a sprite's identifier stored in the world matrix
World.prototype.getGridVal = function(x, y){

	return this.grid[x][y];

}



//Class for all characters and objects 
function Sprite(world, width, height, image, x, y, rx, ry, id){

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
	this.relativeX = rx;				//Position relative to the entire game world
	this.relativeY = ry;
	this.formerX = this.relativeX;	//Previous relative position
	this.formerY = this.relativeY;
	this.dx = 0;					//Amount that x and y are changing by
	this.dy = 0;
	this.moveDistance = 0;
	this.jumpDistance = 0;

	this.visible = true;
	this.hasAnimation = false;
	this.alwaysAnim = false;
	this.alive = true;
	this.onBoundary = STOP;

}

Sprite.prototype.setX = function(x){

	this.x = x;

}

Sprite.prototype.setY = function(y){

	this.y = y;
	
}

Sprite.prototype.moveX = function(dx){

	this.x += dx;
	this.relativeX += dx;

}

Sprite.prototype.moveY = function(dy){

	this.y += dy;
	this.relativeY += dy;
	
}

Sprite.prototype.setRelativeX = function(x){

	this.relativeX = x;

}

Sprite.prototype.setRelativeY = function(y){

	this.relativeY = y;
	
}

Sprite.prototype.setDX = function(dx){

	this.dx = dx;

}

Sprite.prototype.setDY = function(dy){

	this.dy = dy;

}

Sprite.prototype.setMoveDistance = function(moveDistance){

	this.moveDistance = moveDistance;

}

Sprite.prototype.setJumpDistance = function(jumpDistance){

	this.jumpDistance = jumpDistance;
	
}

Sprite.prototype.setId = function(id){

	this.idNum = id;

}

Sprite.prototype.setImage = function(image){

	this.image.src = image;

}

Sprite.prototype.setAlwaysAnim = function(val){

	this.alwaysAnim = val;

}

Sprite.prototype.setOnBoundary = function(boundaryAction){

	this.onBoundary = boundaryAction;

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

Sprite.prototype.getDX = function(){

	return this.dx;

}

Sprite.prototype.getDY = function(){

	return this.dy;
	
}

Sprite.prototype.getMoveDistance = function(){

	return this.moveDistance;

}

Sprite.prototype.getJumpDistance = function(){

	return this.jumpDistance;
	
}

Sprite.prototype.getId = function(){

	return this.idNum;

}

Sprite.prototype.getImage = function(){

	return this.image.src;

}

Sprite.prototype.getAlwaysAnim = function(){

	return alwaysAnim;
	
}

Sprite.prototype.getOnBoundary = function(){

	return this.onBoundary;
	
}

Sprite.prototype.fall = function(){

	this.setDY(this.dy + this.world.gAccel);

}

//Moves the sprite to a new location
Sprite.prototype.changePosition = function(x, y){

	this.setRelativeX(this.relativeX + (x - this.x));
	this.setRelativeY(this.relativeY + (y - this.y));
	this.setX(x);
	this.setY(y);

	if(this.idNum > 0){

		if(this.world.tileWidth > 1)
			this.world.setGridVal(this.relativeX, this.relativeY, this);
		else
			changeWorldPosition();

	}

}

//Changes the sprite's location in the world's grid
Sprite.prototype.changeWorldPosition = function(){

	var i, j;

	if(this.world.tileWidth > 1){

		this.world.setGridVal(this.formerX, this.formerY, null);
		this.world.setGridVal(this.relativeX, this.relativeY, this);

	}else{

		for(i = this.formerX; i < this.formerX + this.width; i++)
			for(j = this.formerY; j < this.formerY + this.height; j++)
				this.world.setGridVal(i, j, null);

		for(i = this.relativeX; i < this.relativeX + this.width; i++)
			for(j = this.relativeY; j < this.relativeY + this.height; j++)
				this.world.setGridVal(i, j, this);

	}

	this.formerX = this.relativeX;
	this.formerY = this.relativeY;

}

//Changes the sprite to resemble another sprite of the same kind
Sprite.prototype.transform = function(id, image){

	this.setId(id);
	this.setImage(image);

	if(this.idNum > 0)
		this.world.setGridVal(this.x, this.y, this);

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

//Collision detection based on the world grid
Sprite.prototype.checkCollision = function(side){

    var collisionSprites = new Array(256);
    var nextSprite = null;
    var collisionCount = 0;
    var sameObject = false;
    var i, j;

    collisionSprites[0] = null;
    if(this.visible){

        //Checks if this sprite is colliding with the top of another sprite
        if((side == "top" || side == "all") && this.relativeY + this.height < this.world.height){

            for(i = this.relativeX; i < this.relativeX + this.width; i++){

            	nextSprite = this.world.getGridVal(i, this.relativeY + this.height);
            	if(nextSprite != null){

                    for(j = 0; j < collisionCount; j++)
                    	if(collisionSprites[j] == nextSprite)
                    		sameObject = true;

                    if(!sameObject){

                    	collisionSprites[collisionCount] = nextSprite;
                    	collisionCount++;

                    }else{

                    	sameObject = false;

                    }

                }

            }

        }

        //Checks if this sprite is colliding with the bottom of another sprite
        if((side == "bottom" || (side == "all" && collisionSprites[0] == null)) && this.relativeY > 0){

            for(i = this.relativeX; i < this.relativeX + this.width; i++){

            	nextSprite = this.world.getGridVal(i, this.relativeY - 1);
            	if(nextSprite != null){

                    for(j = 0; j < collisionCount; j++)
                    	if(collisionSprites[j] == nextSprite)
                    		sameObject = true;

                    if(!sameObject){

                    	collisionSprites[collisionCount] = nextSprite;
                    	collisionCount++;

                    }else{

                    	sameObject = false;

                    }

                }

            }

        }

        //Checks if this sprite is colliding with the left side of another sprite
        if((side == "left" || (side == "all" && collisionSprites[0] == null)) && this.relativeX + this.width < this.world.width){

            for(i = this.relativeY; i < this.relativeY + this.height; i++){

            	nextSprite = this.world.getGridVal(this.relativeX + this.width, i);
            	if(nextSprite != null){

                    for(j = 0; j < collisionCount; j++)
                    	if(collisionSprites[j] == nextSprite)
                    		sameObject = true;

                    if(!sameObject){

                    	collisionSprites[collisionCount] = nextSprite;
                    	collisionCount++;

                    }else{

                    	sameObject = false;

                    }

                }

            }

        }

        //Checks if this sprite is colliding with the right side of another sprite
        if((side == "right" || (side == "all" && collisionSprites[0] == null)) && this.relativeX > 0){

            for(i = this.relativeY; i < this.relativeY + this.height; i++){

            	nextSprite = this.world.getGridVal(this.relativeX - 1, i);
            	if(nextSprite != null){

                    for(j = 0; j < collisionCount; j++)
                    	if(collisionSprites[j] == nextSprite)
                    		sameObject = true;

                    if(!sameObject){

                    	collisionSprites[collisionCount] = nextSprite;
                    	collisionCount++;

                    }else{

                    	sameObject = false;

                    }

                }

            }

        }

    }

    return collisionSprites;

}

//Checks how far down the next sprite is and adjust dy if necessary
Sprite.prototype.checkCollisionDistance = function(){

    var collision = false;
    var i;

    if(this.visible){

        i = this.relativeX;
        while(collision == false && i < this.relativeX + this.width){

            for(j = 0; j < this.dy; j++){

                if(this.world.getGridVal(i, this.relativeY + this.height + j) != null ||
                   this.relativeY + this.height + j >= this.world.height){

                    this.dy = j;
                    collision = true;

                }

            }
            i++;

        }

    }

}

//Performs an action on the sprite when they reach the end of the world
Sprite.prototype.checkBoundary = function(direction){

	var nextLocation = 0;
	var dimension = 0;

	if(direction == "left"){

		nextLocation = this.relativeX - this.moveDistance;
		dimension = this.world.width;

	}else if(direction == "right"){

		nextLocation = this.relativeX + (this.width - 1) + this.moveDistance;
		dimension = this.world.width;

	}else if(direction == "up"){

		nextLocation = this.relativeY - this.moveDistance;
		dimension = this.world.height;

	}else if(direction == "down"){

		nextLocation = this.relativeY + (this.height - 1) + this.moveDistance;
		dimension = this.world.height;

	}

	if(nextLocation < 0 || nextLocation > dimension - 1){

		if(this.onBoundary == STOP)
			return STOP;

	}else{

		return 0;

	}

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

//Cycles through a spritesheet, drawing the frames for an animation
Sprite.prototype.animDraw = function(){

	var xPos = (this.x * this.world.tileWidth) + (this.width / 2);
	var yPos = (this.y * this.world.tileWidth) + (this.height / 2);

	if(((keyPressed[LEFT] || keyPressed[RIGHT] || keyPressed[UP] || keyPressed[DOWN]) || this.alwaysAnim) && this.animTimer.getElapsedTime() >= 1){

		this.animIndex = (this.animIndex + 1) % this.animOrder.length;
		this.currentFrame[1] = this.animOrder[this.animIndex];
		this.animTimer.setElapsedTime(0);

	}

	this.context.translate(xPos, yPos);
	this.context.drawImage(this.image, this.currentFrame[1] * this.frameWidth, this.currentFrame[0] * this.frameHeight,
		                   this.frameWidth, this.frameHeight, 0 - (this.width / 2) + 1, 0 - (this.height / 2) + 1,
		                   this.width, this.height);

}

//Draws the sprite
Sprite.prototype.draw = function(){

	var xPos = (this.x * this.world.tileWidth) + (this.width / 2);
	var yPos = (this.y * this.world.tileWidth) + (this.height / 2);
	
	this.context.translate(xPos, yPos);
	this.context.drawImage(this.image, 0 - (this.width / 2) + 1, 0 - (this.height / 2) + 1, this.width - 1, this.height - 1);

}

//Draws the sprite
//Isometric version which rotates the image to appear 3D
Sprite.prototype.drawIso = function(xScale, yScale, angle, xOffset, yOffset, xTrans, yTrans){

	var xPos = (this.x * this.world.tileWidth) + (this.width / 2) + xOffset + (xOffset / 2);
	var yPos = (this.y * this.world.tileWidth) + (this.height / 2) - yOffset;

	this.context.save();

	this.context.translate(xPos - xTrans, yPos + yTrans);
	this.context.scale(xScale, yScale);
	if(angle != null)
    	this.context.rotate(angle * Math.PI /180);

	this.context.drawImage(this.image, 0 - (this.width / 2) + 1, 0 - (this.height / 2) + 1, this.width - 1, this.height - 1);

	this.context.restore();

}

//Cycles through a spritesheet, drawing the frames for an animation
//Isometric version which rotates the image to appear 3D
Sprite.prototype.animDrawIso = function(xScale, yScale, angle, xOffset, yOffset, xTrans, yTrans){

	var xPos = (this.x * this.world.tileWidth) + (this.width / 2) + xOffset + (xOffset / 2);
	var yPos = (this.y * this.world.tileWidth) + (this.height / 2) - yOffset;

	this.context.save();

	if(((keyPressed[LEFT] || keyPressed[RIGHT] || keyPressed[UP] || keyPressed[DOWN]) || this.alwaysAnim) && this.animTimer.getElapsedTime() >= 1){

		this.animIndex = (this.animIndex + 1) % this.animOrder.length;
		this.currentFrame[1] = this.animOrder[this.animIndex];
		this.animTimer.setElapsedTime(0);

	}

	this.context.translate(xPos - xTrans, yPos + yTrans);
	this.context.scale(xScale, yScale);
	if(angle != null)
    	this.context.rotate(angle * Math.PI /180);

	this.context.drawImage(this.image, this.currentFrame[1] * this.frameWidth, this.currentFrame[0] * this.frameHeight,
		                   this.frameWidth, this.frameHeight, 0 - (this.width / 2) + 1, 0 - (this.height / 2) + 1,
		                   this.width, this.height);

	this.context.restore();

}

Sprite.prototype.update = function(){

	var i, j;

	if(this.visible){

		if(this.idNum > 0)
			for(i = this.relativeX; i < this.relativeX + this.width; i++)
				for(j = this.relativeY; j < this.relativeY + this.height; j++)
					if(this.world.grid[i][j] == null)
						this.world.setGridVal(i, j, this);

		this.context.save();

		if(this.hasAnimation)
			this.animDraw();
		else
			this.draw();

		this.context.restore();

	}

}

//Removes a sprite from the game
Sprite.prototype.destroy = function(){

	//Resets sprite's previous position in the world grid
	if(this.world.tileWidth > 1){

		this.world.setGridVal(this.x, this.y, null);

	}else{

		for(i = this.relativeX; i < this.relativeX + this.width; i++)
			for(j = this.relativeY; j < this.relativeY + this.height; j++)
				this.world.setGridVal(i, j, null);

	}

	this.alive = false;

}



//Class for destructible non-player sprites
function Destructible(world, width, height, image, x, y, rx, ry, id, hitPoints){

	var hp = hitPoints;
	var theDestructible = new Sprite(world, width, height, image, x, y, rx, ry, id);

	theDestructible.damage = function(damage){

		hp = hp - damage;

	}

	theDestructible.getHP = function(){

		return hp;

	}

	theDestructible.setHP = function(hitPoints){

		hp = hitPoints;

	}

	return theDestructible;

}



//Class for a more advanced sprite
function AdvancedSprite(world, width, height, image, x, y, rx, ry, id){

	var hp = 0;
	var inventory = null;
	var abilities = null;
	var abilityIndex = 0;
	var theSprite = new Sprite(world, width, height, image, x, y, rx, ry, id);

	theSprite.damage = function(damage){

		hp = hp - damage;

	}

	theSprite.getHP = function(){

		return hp;

	}

	theSprite.setHP = function(hitPoints){

		hp = hitPoints;

	}

	theSprite.initInventory = function(){

		inventory = new Array(256);

	}

	theSprite.addToInventory = function(item){

		var index = 0;

		while(inventory[index] != null)
			index++;

		inventory[index] = item;

	}

	theSprite.removeFromInventory = function(index){

		inventory[index] = null;

	}

	theSprite.getItem = function(index){

		return inventory[index];

	}

	theSprite.initAbilities = function(){

		abilities = new Array(256);

	}

	theSprite.addToAbilities = function(item){

		var index = 0;

		while(abilities[index] == null)
			index++;

		abilities[index] = item;

	}

	theSprite.removeFromAbilities = function(index){

		abilities[index] = null;

	}

	theSprite.getAbility = function(index){

		return theSprite.abilities[index];

	}

	return theSprite;

}



//Class for items stored in the AdvancedSprite's inventory
function Item(desc, id){

	this.description = desc;
	this.idNum = id;

}

Item.prototype.setDescription = function(desc){

	this.description = desc;

}

Item.prototype.getDescription = function(){

	return this.description;

}

Item.prototype.setId = function(id){

	this.idNum = id;

}

Item.prototype.getId = function(){

	return this.idNum;
	
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
	this.updateTime = setInterval(function(){

		self.elapsedTime++;

	}, this.interval);

}

Timer.prototype.startCountDown = function(){

	//Set the timer to count down every "interval" milliseconds
	this.updateTime = setInterval(function(){

		this.currentTime--;

	}, this.interval);

}

Timer.prototype.stop = function(){

	//Stop the timer
	clearInterval(this.updateTime);

}

Timer.prototype.getCurrentTime = function(){

	return this.currentTime;

}

Timer.prototype.setCurrentTime = function(time){

	this.currentTime = time;

}

Timer.prototype.getElapsedTime = function(){

	return self.elapsedTime;

}

Timer.prototype.setElapsedTime = function(time){

	this.elapsedTime = time;

}