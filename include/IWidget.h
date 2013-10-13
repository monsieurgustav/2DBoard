//
//  IWidget.h
//  Labyrinth
//
//  Created by Guillaume Buisson on 06/10/13.
//
//

#ifndef Labyrinth_IWidget_h
#define Labyrinth_IWidget_h

#include "cinder/app/App.h"


/// An interface to intercept draw/update/interaction.
class IWidget
{
public:
    enum State
    {
        KEEP,
        REMOVE,
    };

public:
    virtual ~IWidget()
    { }

    /*
     * Must return either:
     * - KEEP to let the widget in the list, or
     * - REMOVE to remove and delete it.
     */
    virtual State update() = 0;

    virtual void beforeDraw(cinder::Vec2i windowSize)
    { }

    virtual void afterDraw(cinder::Vec2i windowSize)
    { }
    
    /// return true if the event has been consumed.
    virtual bool keyDown(cinder::app::KeyEvent event)
    {
        return false;
    }

    /// return true if the event has been consumed.
    virtual bool keyUp(cinder::app::KeyEvent event)
    {
        return false;
    }

    /// return true if the event has been consumed.
	virtual bool mouseDown(cinder::app::MouseEvent event)
    {
        return false;
    }
    /// return true if the event has been consumed.
	virtual bool mouseUp(cinder::app::MouseEvent event)
    {
        return false;
    }

    /// return true if the event has been consumed.
	virtual bool mouseWheel(cinder::app::MouseEvent event)
    {
        return false;
    }

    /// return true if the event has been consumed.
	virtual bool mouseMove(cinder::app::MouseEvent event)
    {
        return false;
    }

    /// return true if the event has been consumed.
	virtual bool mouseDrag(cinder::app::MouseEvent event)
    {
        return false;
    }
};

#endif
