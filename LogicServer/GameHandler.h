#ifndef GameHandler_h
#define GameHandler_h
#include "LBase.h"
#include "User.h"
#include "Card.h"
#include <list>

class Desk;

class GameHandler
{
public:
	virtual ~GameHandler(){}
    virtual bool startup(Desk *game) { return false; }
    virtual void shutdown(void) {}
    virtual void gameStart(void) {}
	virtual int getCurPos() { return 0; }
	virtual int getUserScore(Lint pos) { return 0; }

	virtual void			HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg){}
	virtual void			HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg){}
	virtual void			HanderUserEndSelect(User*	pUser,LMsgC2SUserEndCardSelect* msg){}		//玩家处理海底牌
	virtual void			HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg){}
	virtual void			OnUserReconnect(User* pUser){}
	virtual void			OnGameOver(Lint result,Lint bombpos){}
	virtual void			OnGameOver(Lint result,Lint winpos[],Lint bombpos, CardVector winCards[]){}
	virtual void			SetDeskPlay(){}
	virtual void    		ProcessRobot(Lint pos, User * pUser){}
	virtual void           ProcessAutoPlay(Lint pos, User * pUser){}
	virtual void           HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg){}

	virtual void           HanderUserTangReq(User* pUser,LMsgC2STangCard* msg) {}

	// 玩家定缺
	virtual void			HanderUserDingQue(User* pUser, LMsgC2SUserDingQue* msg){}
	// 玩家买跑
	virtual void			HanderUserChange(User* pUser, LMsgC2SBottomPour* msg){}

protected:
	 PlayType m_playtype;	// 桌子的玩法
	 
};

class GameHandlerCreator
{
public:
    virtual GameHandler* create(void) = 0;
};

class GameHandlerFactory
{
public:
    GameHandlerFactory(void);
    ~GameHandlerFactory(void);

    static GameHandlerFactory* getInstance(void);

    GameHandler* create(GameType gameType);
    void destroy(GameType gameType, GameHandler *gameHandler);

    void registerCreator(GameType gameType, GameHandlerCreator *creator);
    void unregisterCreator(GameType gameType);

private:
    std::map<GameType, GameHandlerCreator*> mCreators;
    std::map<GameType, std::list<GameHandler*> > mFreeHandlers;
};


#define DECLARE_GAME_HANDLER_CREATOR(gameType, className)                           \
class GameHandlerCreator_##className : public GameHandlerCreator                    \
{                                                                                   \
public:                                                                             \
    GameHandlerCreator_##className(void)                                            \
    {                                                                               \
        GameHandlerFactory::getInstance()->registerCreator(gameType,                \
            (GameHandlerCreator*)this);                                             \
    }                                                                               \
    ~GameHandlerCreator_##className(void)                                           \
    {                                                                               \
        GameHandlerFactory::getInstance()->unregisterCreator(                       \
            gameType);                                                              \
    }                                                                               \
    GameHandler* create(void)                                                       \
    {                                                                               \
        return new className();                                                     \
    }                                                                               \
};                                                                                  \
static GameHandlerCreator_##className sGameHandlerCreator_##className;
#endif
