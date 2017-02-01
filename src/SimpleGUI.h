// MowaLibs
//
// Copyright (c) 2011, Marcin Ignac / marcinignac.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// This code is intended to be used with the Cinder C++ library, http://libcinder.org
//
// Temptesta Seven font by Yusuke Kamiyamane http://p.yusukekamiyamane.com/fonts/
// "The fonts can be used free for any personal or commercial projects."

#pragma once

#include <vector>
#include "cinder/app/App.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"
#include "cinder/Function.h"

namespace mowa { namespace sgui {

//-----------------------------------------------------------------------------

class Control;
class FloatVarControl;
class IntVarControl;
class BoolVarControl;
class ButtonControl;
class LabelControl;
class SeparatorControl;
class ColumnControl;
class PanelControl;
class TextureVarControl;
class ColorVarControl;

//-----------------------------------------------------------------------------

class SimpleGUI {
private:
	bool enabled;
	cinder::vec2	mousePos;
	std::vector<Control*> controls;
	Control* selectedControl;

    ci::signals::ScopedConnection  cbMouseDown, cbMouseUp, cbMouseDrag;

    void	init(ci::app::App* app);
public:
	static ci::ColorA darkColor;
	static ci::ColorA lightColor;
    static ci::ColorA bgColor;
	static ci::ColorA textColor;
	static float spacing;
	static ci::vec2 padding;
	static ci::vec2 sliderSize;
	static ci::vec2 labelSize;
	static ci::vec2 separatorSize;
	static ci::Font textFont;

	enum {
		RGB,
		HSV
	};
public:
    SimpleGUI(ci::app::App* app);
	bool	isSelected() { return selectedControl != NULL; }
	std::vector<Control*>& getControls() { return controls; }

	bool	onMouseDown(ci::app::MouseEvent event);
    bool	onMouseUp(ci::app::MouseEvent event);
    bool	onMouseDrag(ci::app::MouseEvent event);

	void	draw();
	void	dump();
	void	save(std::string fileName = "");
	void	load(std::string fileName = "");

	bool	isEnabled();
	void	setEnabled(bool state);

	FloatVarControl* 	addParam(const std::string& paramName, float* var, float min=0, float max=1, float defaultValue = 0);
	IntVarControl*		addParam(const std::string& paramName, int* var, int min=0, int max=1, int defaultValue = 0);
	BoolVarControl*		addParam(const std::string& paramName, bool* var, bool defaultValue = false, int groupId = -1);
    ColorVarControl*	addParam(const std::string& paramName, ci::ColorA* var, ci::ColorA const defaultValue = ci::ColorA(0, 1, 1, 1), int colorModel = RGB);
    TextureVarControl*	addParam(const std::string& paramName, ci::gl::Texture* var, int scale = 1, bool flipVert = false);

	ButtonControl*		addButton(const std::string& buttonName);
	LabelControl*		addLabel(const std::string& labelName);
	SeparatorControl*	addSeparator();
	ColumnControl*		addColumn(int x = 0, int y = 0);
	PanelControl*		addPanel();

	Control*			getControlByName(const std::string& name);

    static ci::vec2		getStringSize(const std::string& str);
    static ci::Rectf		getScaledWidthRectf(ci::Rectf rect, float scale);
};

//-----------------------------------------------------------------------------


class Control {
public:
	enum Type {
		FLOAT_VAR,
		INT_VAR,
		BOOL_VAR,
		COLOR_VAR,
		TEXTURE_VAR,
		BUTTON,
		LABEL,
		SEPARATOR,
		COLUMN,
		PANEL
	};

	ci::vec2	position;
	ci::Rectf	activeArea;
	ci::ColorA	bgColor;
	Type	type;
	std::string name;
	SimpleGUI* parentGui;

	Control();
	virtual ~Control() {};
	void setBackgroundColor(ci::ColorA color);
	void notifyUpdateListeners();
	virtual ci::vec2 draw(ci::vec2 pos) = 0;
	virtual std::string toString() { return ""; };
	virtual void fromString(std::string& strValue) {};
    virtual void onMouseDown(ci::app::MouseEvent event) {};
    virtual void onMouseUp(ci::app::MouseEvent event) {};
    virtual void onMouseDrag(ci::app::MouseEvent event) {};
};

//-----------------------------------------------------------------------------

class FloatVarControl : public Control {
public:
	float* var;
	float min;
	float max;
public:
	FloatVarControl(const std::string& name, float* var, float min=0, float max=1, float defaultValue = 0);
	float getNormalizedValue();
	void setNormalizedValue(float value);
	ci::vec2 draw(ci::vec2 pos);
	std::string toString();
	void fromString(std::string& strValue);
    void onMouseDown(ci::app::MouseEvent event);
    void onMouseDrag(ci::app::MouseEvent event);
};

//-----------------------------------------------------------------------------

class IntVarControl : public Control {
public:
	int* var;
	int min;
	int max;
public:
	IntVarControl(const std::string& name, int* var, int min=0, int max=1, int defaultValue = 0);
	float getNormalizedValue();
	void setNormalizedValue(float value);
	ci::vec2 draw(ci::vec2 pos);
	std::string toString();
	void fromString(std::string& strValue);
    void onMouseDown(ci::app::MouseEvent event);
    void onMouseDrag(ci::app::MouseEvent event);
};

//-----------------------------------------------------------------------------

class BoolVarControl : public Control {
public:
	bool* var;
	int groupId;
public:
	BoolVarControl(const std::string& name, bool* var, bool defaultValue, int groupId);
	ci::vec2 draw(ci::vec2 pos);
	std::string toString();
	void fromString(std::string& strValue);
    void onMouseDown(ci::app::MouseEvent event);
};

//-----------------------------------------------------------------------------

class ColorVarControl : public Control {
public:
	ci::ColorA* var;
	ci::Rectf	activeArea1;
	ci::Rectf	activeArea2;
	ci::Rectf	activeArea3;
	ci::Rectf	activeArea4;
	int		activeTrack;
	int		colorModel;
public:
	ColorVarControl(const std::string& name, ci::ColorA* var, ci::ColorA defaultValue, int colorModel);
	ci::vec2 draw(ci::vec2 pos);
	std::string toString();	//saved as "r g b a"
	void fromString(std::string& strValue); //expecting "r g b a"
    void onMouseDown(ci::app::MouseEvent event);
    void onMouseDrag(ci::app::MouseEvent event);
};

//-----------------------------------------------------------------------------

class ButtonControl : public Control {
private:
	bool pressed;
    ci::CallbackMgr<bool (ci::app::MouseEvent)>		callbacksClick;
public:
	ButtonControl(const std::string& name);
	ci::vec2 draw(ci::vec2 pos);
    void onMouseDown(ci::app::MouseEvent event);
    void onMouseUp(ci::app::MouseEvent event);

	//! Registers a callback for Click events. Returns a unique identifier which can be used as a parameter to unregisterClick().
    ci::CallbackId		registerClick( std::function<bool (ci::app::MouseEvent)> callback ) { return callbacksClick.registerCb( callback ); }
	//! Registers a callback for Click events. Returns a unique identifier which can be used as a parameter to unregisterClick().
	template<typename T>
    ci::CallbackId		registerClick( T *obj, bool (T::*callback)(ci::app::MouseEvent) ) { return callbacksClick.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
	//! Unregisters a callback for Click events.
	void			unregisterClick( ci::CallbackId id ) { callbacksClick.unregisterCb( id ); }

	void fireClick();

};

//-----------------------------------------------------------------------------

class LabelControl : public Control {
public:
	LabelControl(const std::string& name);
	void setText(const std::string& text);
    ci::vec2 draw(ci::vec2 pos);
};

//-----------------------------------------------------------------------------

class SeparatorControl : public Control {
public:
	SeparatorControl();
	ci::vec2 draw(ci::vec2 pos);
};

//-----------------------------------------------------------------------------

class ColumnControl : public Control {
public:
	int x;
	int y;
	ColumnControl(int x = 0, int y = 0);
	ci::vec2 draw(ci::vec2 pos);
};

//-----------------------------------------------------------------------------

class PanelControl : public Control {
public:
	bool enabled;
	PanelControl();
	ci::vec2 draw(ci::vec2 pos);
};


//-----------------------------------------------------------------------------

class TextureVarControl : public Control {
public:
	ci::gl::Texture* var;
	float scale;
	bool flipVert;
	TextureVarControl(const std::string& name, ci::gl::Texture* var, int scale, bool flipVert = false);
	ci::vec2 draw(ci::vec2 pos);
};

//-----------------------------------------------------------------------------

} //namespace sgui
} //namespace vrg
