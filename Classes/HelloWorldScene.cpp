#include "HelloWorldScene.h"

USING_NS_CC;


Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	if ( !Layer::init() )
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	auto labelStart = Label::createWithSystemFont("Play Game", "Arial", 30);
    
	auto startItem = MenuItemLabel::create(labelStart, CC_CALLBACK_1(HelloWorld::menuCallback, this));
	startItem->setTag(START);
	startItem->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height-150));

	auto menu = Menu::create(startItem,  NULL); 
	menu->setPosition(Point::ZERO);
	this->addChild(menu, 1);

	auto sprite = Sprite::create("sand.png");

	sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(sprite, 0);

    if(highScore>0){
        char scoreCount[20] = {0};
        sprintf(scoreCount,"High Score: %d",highScore);
        auto labelHighScore = Label::createWithSystemFont(scoreCount, "Arial", 16);
        labelHighScore->setPosition(Point(visibleSize.width/2, visibleSize.height-200));
        this->addChild(labelHighScore);
    }
    
    Director::getInstance()->setDisplayStats(false);
	return true;
}


void HelloWorld::menuCallback(Ref* pSender)
{
	switch (((Node*)pSender)->getTag()) {
	case START:         
		CCLOG("go to game");
		Director::getInstance()->replaceScene(GameScene::createScene());
		break;     
	case EXIT:         
		Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)         
		exit(0); 
#endif 
	}
}

bool GameScene::init() 
{     
	if (!Layer::init())     {
		return false;     
	}       
	Size visibleSize = Director::getInstance()->getVisibleSize();  
	Point origin = Director::getInstance()->getVisibleOrigin();


	auto sprite = Sprite::create("sand.png");

	sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	this->addChild(sprite, 0);
    
    this->labelScore = Label::createWithSystemFont("Score: 0", "Arial", 16);
	this->labelScore->setPosition(Point(visibleSize.width- labelScore->getContentSize().width, visibleSize.height - labelScore->getContentSize().height));
	this->addChild(labelScore);
    
    winWidth=visibleSize.width;
    winHeight=visibleSize.height;
    score=0;
	speed=0.09f;
    gameOverFlag=false;

    snakeLength=3;
    snakeWidth=18;
    snakeStep=2;
    snakeAllStep=0;
    controlDir=DIR_DEF::LEFT;
    int x=winWidth-snakeLength*snakeWidth-10;
    int y=rand()%winHeight-snakeWidth;
    x=(x/snakeWidth)*snakeWidth;
    y=(y/snakeWidth)*snakeWidth;
    

    for(int i=0;i<snakeLength;i++){
        auto sprite = Sprite::create("snake.png");
        auto node = new SnakeNode();
        node->x=x+snakeWidth*i;
        node->y=y;
        int dir=controlDir;
        node->sprite=sprite;
        node->sprite->setPosition(Point(node->x, node->y));
        node->sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
        

        this->addChild(node->sprite);
        snake.pushBack(node);
        if(i>0){
            for(int j=0;j<snakeStep;j++){
                auto dirNode = new DirNode();
                dirNode->dir=dir;
                dirChain.pushBack(dirNode);
            }
        }
    }
    
    food = newFood();
    this->addChild(food->sprite);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener,this);
    

    auto listenerKeyboard = EventListenerKeyboard::create();
    listenerKeyboard->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    listenerKeyboard->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenerKeyboard, this);
    
    this->schedule(SEL_SCHEDULE(&GameScene::play), speed);
    
	return true; 
}

SnakeNode* GameScene::newFood(){
    auto node=new SnakeNode();
    auto sprite = Sprite::create("mouse.png");
    node->x=rand()%winWidth-sprite->getContentSize().width;
    node->y=rand()%winHeight-sprite->getContentSize().height;
    node->sprite=sprite;
    node->sprite->setPosition(Point(node->x, node->y));
    node->sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
    return node;
}
bool GameScene::onTouchBegan( Touch* touch, Event* event )
{
    Point touchPoint = touch->getLocation();
    touch_x= (int)touchPoint.x;
    touch_y= (int)touchPoint.y;
    return true;
}
void GameScene::onTouchEnded( Touch* touch, Event* event )
{
    Point touchPoint = touch->getLocation();
    int x = (int)touchPoint.x;
    int y = (int)touchPoint.y;
    int x1=touch_x;
    int y1=touch_y;
    int dir;
    
    if (abs(y - y1) > abs(x - x1)) {
        if (y > y1) dir = DIR_DEF::UP;
        else dir = DIR_DEF::DOWN;
    }
    else {
        if (x > x1)
            dir = DIR_DEF::RIGHT;
        else dir = DIR_DEF::LEFT;
    }
    snakeMove(dir);
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    log("Key with keycode %d pressed", keyCode);
    
    if(keyCode==EventKeyboard::KeyCode::KEY_LEFT_ARROW) snakeMove(DIR_DEF::LEFT);
    else if(keyCode==EventKeyboard::KeyCode::KEY_UP_ARROW) snakeMove(DIR_DEF::UP);
    else if(keyCode==EventKeyboard::KeyCode::KEY_RIGHT_ARROW) snakeMove(DIR_DEF::RIGHT);
    else if(keyCode==EventKeyboard::KeyCode::KEY_DOWN_ARROW) snakeMove(DIR_DEF::DOWN);
    
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    log("Key with keycode %d released", keyCode);
}

void GameScene::snakeMove(int dir){
    if(controlDir==DIR_DEF::UP && dir == DIR_DEF::DOWN);
    else if(controlDir==DIR_DEF::DOWN && dir == DIR_DEF::UP);
    else if(controlDir==DIR_DEF::LEFT && dir == DIR_DEF::RIGHT);
    else if(controlDir==DIR_DEF::RIGHT && dir == DIR_DEF::LEFT);
    else controlDir=dir;
}
void GameScene::play(float t)
{
    if(!gameOverFlag){
        for(int i=snake.size()-1;i>=0;i--){
            SnakeNode *node=snake.at(i);
            int x=node->x;
            int y=node->y;
            int d=controlDir;
            int step=snakeWidth/snakeStep;
            if(i==0) d=controlDir;
            else {
                DirNode *dirNode =dirChain.at((i-1)*snakeStep+snakeStep-1);
                d=dirNode->dir;
            }
            switch (d)
            {
                case DIR_DEF::UP:
                    y +=step;
                    break;
                case DIR_DEF::DOWN:
                    y -=step;
                    break;
                case DIR_DEF::LEFT:
                    x -=step;
                    break;
                case DIR_DEF::RIGHT:
                    x +=step;
                    break;
            }
            
            node->x=x;
            node->y=y;
            
            float duration =speed*0.8f;
            auto actionMove = MoveTo::create(duration,
                                             Point(node->x, node->y));
            
            node->sprite->runAction(Sequence::create(actionMove, NULL, NULL));
            
            
        }

        for(int i=dirChain.size()-1;i>=0;i--){
            DirNode *dirNode=dirChain.at(i);
            int d=dirNode->dir;
            if(i==0) d=controlDir;
            else {
                DirNode *lastDirNode =dirChain.at(i-1);
                d=lastDirNode->dir;
            }
            dirNode->dir=d;
        }
        
        SnakeNode *head=snake.at(0);

        if(collisionDetection(head,food)){
            score+=1;
            if(score>highScore) highScore=score;
            this->removeChild(labelScore);
            char scoreCount[20] = {0};
            sprintf(scoreCount,"Score: %d",score);
            labelScore = Label::createWithSystemFont(scoreCount, "Arial", 16);
            
            labelScore->setPosition(Point(winWidth-labelScore->getContentSize().width,
                                          winHeight-labelScore->getContentSize().height));
            this->addChild(labelScore);


            auto tail=snake.at(snake.size()-1);
            auto sprite = Sprite::create("snake.png");
            auto node = new SnakeNode();
            Point pt=tail->sprite->getPosition();
            node->x=tail->x;
            node->y=tail->y;
            node->sprite=sprite;
            node->sprite->setPosition(Point(node->x, node->y));
            node->sprite->setAnchorPoint(Point::ANCHOR_BOTTOM_LEFT);
            this->addChild(node->sprite);
            snake.pushBack(node);
            snakeLength++;
            
            for(int j=0;j<snakeStep;j++){
                auto dirNode = new DirNode();
                dirNode->dir=0;
                dirChain.pushBack(dirNode);
            }
            
            this->removeChild(food->sprite);
            food = newFood();
            this->addChild(food->sprite);
            
        }
        else {
            if(borderDetection(head->sprite)) {
                gameOverFlag=true;
            }
            for(int i=2;i<snake.size();i++){
                SnakeNode *node=snake.at(i);
                if(collisionDetection(head,node)) {
                    gameOverFlag=true;
                    break;
                }
            }
        }
        snakeAllStep++;
    }
    else {
		gameOver();
    }
}

void GameScene::gameOver(){
    CCLOG("Game Over");
    Director::getInstance()->replaceScene(HelloWorld::createScene());
}

Scene* GameScene::createScene()
{
	auto scene = Scene::create();    
	auto layer = GameScene::create();     
	scene->addChild(layer);     
	return scene; 
}   

void GameScene::menuBackToMain( cocos2d::Ref* pSender ) {
	Director::getInstance()->replaceScene(HelloWorld::createScene());
}

bool GameScene::borderDetection(cocos2d::Sprite* sprite){
    Point pt=sprite->getPosition();
    Size sz=sprite->getContentSize();
    if(pt.x<0 || pt.y<0 || pt.x+sz.width>=winWidth || pt.y+sz.height>=winHeight) return true;
    else return false;
}

bool GameScene::collisionDetection(SnakeNode* node1,SnakeNode* node2){
    Size sz1=node1->sprite->getContentSize();
    int p1x1=node1->x;
    int p1y1=node1->y;
    int p1x2=p1x1+sz1.width-1;
    int p1y2=p1y1+sz1.height-1;
    Size sz2=node2->sprite->getContentSize();
    int p2x1=node2->x;
    int p2y1=node2->y;
    int p2x2=p2x1+sz2.width-1;
    int p2y2=p2y1+sz2.height-1;
    if(p1x1>p2x2 || p1x2<p2x1 || p1y1>p2y2 || p1y2<p2y1) return false;
    else return true;
}