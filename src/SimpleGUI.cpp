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

#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "SimpleGUI.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Font.h"
#include "cinder/CinderMath.h"

namespace mowa { namespace sgui {

//-----------------------------------------------------------------------------

ci::Font SimpleGUI::textFont = ci::Font();
ci::ColorA SimpleGUI::darkColor = ci::ColorA(0.3, 0.3, 0.3, 1.0);
ci::ColorA SimpleGUI::lightColor = ci::ColorA(1, 1, 1.0, 1.0);
ci::ColorA SimpleGUI::bgColor = ci::ColorA(0, 0, 0, 0.5);
ci::ColorA SimpleGUI::textColor = ci::ColorA(1, 1, 1, 1);
float SimpleGUI::spacing = 7;
ci::vec2 SimpleGUI::padding = ci::vec2(3, 3);
ci::vec2 SimpleGUI::sliderSize = ci::vec2(125, 10);
ci::vec2 SimpleGUI::labelSize = ci::vec2(125, 10);
ci::vec2 SimpleGUI::separatorSize = ci::vec2(125, 1);

    SimpleGUI::SimpleGUI(ci::app::App* app) {
    init(app);
    enabled = true;
}

    void SimpleGUI::init(ci::app::App* app) {
    textFont = ci::Font("Arial", 12);
    selectedControl = NULL;
    auto onMouseDown = [this](ci::app::MouseEvent event) -> bool { SimpleGUI::onMouseDown(event); return false;};
    auto onMouseUp = [this](ci::app::MouseEvent event) -> bool { SimpleGUI::onMouseUp(event); return false;};
    auto onMouseDrag = [this](ci::app::MouseEvent event) -> bool { SimpleGUI::onMouseDrag(event); return false;};
    cbMouseDown = app->getWindow()->getSignalMouseDown().connect(onMouseDown);
    cbMouseUp = app->getWindow()->getSignalMouseUp().connect(onMouseUp);
    cbMouseDrag = app->getWindow()->getSignalMouseDrag().connect(onMouseDrag);
}

FloatVarControl* SimpleGUI::addParam(const std::string& paramName, float* var, float min, float max, float defaultValue) {
    FloatVarControl* control = new FloatVarControl(paramName, var, min, max, defaultValue);
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

IntVarControl* SimpleGUI::addParam(const std::string& paramName, int* var, int min, int max, int defaultValue) {
    IntVarControl* control = new IntVarControl(paramName, var, min, max, defaultValue);
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

BoolVarControl* SimpleGUI::addParam(const std::string& paramName, bool* var, bool defaultValue, int groupId) {
    BoolVarControl* control = new BoolVarControl(paramName, var, defaultValue, groupId);
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

ColorVarControl* SimpleGUI::addParam(const std::string& paramName, ci::ColorA* var, ci::ColorA defaultValue, int colorModel) {
    ColorVarControl* control = new ColorVarControl(paramName, var, defaultValue, colorModel);
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

TextureVarControl* SimpleGUI::addParam(const std::string& paramName, ci::gl::Texture* var, int scale, bool flipVert) {
    TextureVarControl* control = new TextureVarControl(paramName, var, scale, flipVert);
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

ButtonControl* SimpleGUI::addButton(const std::string& buttonName) {
    ButtonControl* control = new ButtonControl(buttonName);
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

LabelControl* SimpleGUI::addLabel(const std::string& labelName) {
    LabelControl* control = new LabelControl(labelName);
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

SeparatorControl* SimpleGUI::addSeparator() {
    SeparatorControl* control = new SeparatorControl();
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

ColumnControl* SimpleGUI::addColumn(int x, int y) {
    ColumnControl* control = new ColumnControl(x, y);
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

PanelControl* SimpleGUI::addPanel() {
    PanelControl* control = new PanelControl();
    control->parentGui = this;
    controls.push_back(control);
    return control;
}

void SimpleGUI::draw() {
    if (!enabled) return;

    ci::gl::pushMatrices();
    ci::gl::setMatricesWindow(ci::app::getWindowSize());
    ci::gl::disableDepthRead();
    ci::gl::disableDepthWrite();
    ci::gl::enableAlphaBlending();

    ci::vec2 position = ci::vec2(spacing, spacing);

    ColumnControl* currColumn = NULL;
    PanelControl* currPanel = NULL;

    std::vector<Control*>::iterator it = controls.begin();
    while (it != controls.end()) {
        Control* control = *it++;
        if (control->type == Control::PANEL) {
            currPanel = (PanelControl*)control;
        }
        if (control->type == Control::COLUMN) {
            if (currColumn == NULL) {  // first column
                position.x = -SimpleGUI::labelSize.x;
                // each column moves everything to the right so we want compensate that
                position.y = 0;
            }
            currColumn = (ColumnControl*)control;
            currPanel = NULL;
        }
        if (currPanel != NULL && !currPanel->enabled) {
            continue;
        }
        position = control->draw(position);
    }

    ci::gl::disableAlphaBlending();
    ci::gl::enableDepthRead();
    ci::gl::enableDepthWrite();
    ci::gl::color(ci::ColorA(1, 1, 1, 1));
    ci::gl::popMatrices();
}

bool SimpleGUI::isEnabled() {
    return enabled;
}

void SimpleGUI::setEnabled(bool state) {
    enabled = state;
}

void SimpleGUI::dump() {
    std::vector<Control*>::iterator it = controls.begin();
    while (it != controls.end()) {
        Control* control = *it++;
        std::string str = control->toString();
        std::cout << control->name << " : " + str << std::endl;
    }
}

void SimpleGUI::save(std::string fileName) {
    ci::app::console() << "SimpleGUI::save " << fileName << std::endl;
    std::vector<Control*>::iterator it = controls.begin();
    std::fstream file_op(fileName.c_str(), std::ios::out);
    while (it != controls.end()) {
        Control* control = *it++;
        std::string str = control->toString();
        file_op << control->name << " : " + str << std::endl;
    }
    file_op.close();
}

void SimpleGUI::load(std::string fileName) {
    ci::app::console() << "SimpleGUI::load " << fileName << std::endl;
    std::fstream file_op(fileName.c_str(), std::ios::in);
    char str[2000];
    if ((file_op.rdstate() & std::ifstream::failbit) != 0) {
        ci::app::console() << "SimpleGUI::load " << "failed" << std::endl;
        return;
    }
    while (!file_op.eof()) {
        file_op.getline(str, 2000);
        std::string line(str);
        size_t colonPos = line.find(":");

        if (colonPos == std::string::npos) {
            continue;
        }

        std::string name = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        boost::trim(name);
        boost::trim(value);

        Control* control = getControlByName(name);
        if (control) {
            control->fromString(value);
        }
    }
    file_op.close();
}


    bool SimpleGUI::onMouseDown(ci::app::MouseEvent event) {
    if (!enabled) return false;

    std::vector<Control*>::iterator it = controls.begin();
    while (it != controls.end()) {
        Control* control = *it++;
        if (control->activeArea.contains(event.getPos())) {
            selectedControl = control;
            selectedControl->onMouseDown(event);
            return true;
        }
    }
    return false;
}

    bool SimpleGUI::onMouseUp(ci::app::MouseEvent event) {
    if (!enabled) return false;

    if (selectedControl != NULL) {
        selectedControl->onMouseUp(event);
        selectedControl = NULL;
        return true;
    }
    return false;
}

    bool SimpleGUI::onMouseDrag(ci::app::MouseEvent event) {
    if (!enabled) return false;

    mousePos = event.getPos();

    if (selectedControl) {
        selectedControl->onMouseDrag(event);
        return true;
    }
    return false;
}

ci::vec2 SimpleGUI::getStringSize(const std::string& str) {
    ci::TextLayout text;
    text.addLine(str);
    ci::Surface s = text.render(true);
    return s.getSize();
}

ci::Rectf SimpleGUI::getScaledWidthRectf(ci::Rectf rect, float scale) {
    return ci::Rectf(
        rect.getX1(),
        rect.getY1(),
        rect.getX1() + rect.getWidth() * scale,
        rect.getY1() + rect.getHeight()
    );
}

Control* SimpleGUI::getControlByName(const std::string& name) {
    std::vector<Control*>::iterator it = controls.begin();
    while (it != controls.end()) {
        Control* control = *it++;
        if (control->name == name) {
            return control;
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------

Control::Control() {
    bgColor = ci::ColorA(0.0, 0.0, 0.0, 0.5);
}

void Control::setBackgroundColor(ci::ColorA color) {
    bgColor = color;
}

//-----------------------------------------------------------------------------

FloatVarControl::FloatVarControl(const std::string& name, float* var, float min, float max, float defaultValue) {
    this->type = Control::FLOAT_VAR;
    this->name = name;
    this->var = var;
    this->min = min;
    this->max = max;
    if (defaultValue < min) {
        *var = min;
    }
    else if (defaultValue > max) {
        *var = max;
    }
    else {
        *var = defaultValue;
    }
}

float FloatVarControl::getNormalizedValue() {
    return (*var - min)/(max - min);
}

void FloatVarControl::setNormalizedValue(float value) {
    float newValue = min + value*(max - min);
    if (newValue != *var) {
        *var = newValue;
    }
}

ci::vec2 FloatVarControl::draw(ci::vec2 pos) {
    activeArea = ci::Rectf(
        pos.x,
        pos.y + SimpleGUI::labelSize.y + SimpleGUI::padding.y,
        pos.x + SimpleGUI::sliderSize.x,
        pos.y + SimpleGUI::labelSize.y + SimpleGUI::padding.y + SimpleGUI::sliderSize.y
    );

    ci::gl::color(SimpleGUI::bgColor);
    ci::gl::drawSolidRect(ci::Rectf(
        (pos - SimpleGUI::padding).x,
        (pos - SimpleGUI::padding).y,
        (pos + SimpleGUI::sliderSize + SimpleGUI::padding).x,
        (pos + SimpleGUI::labelSize + SimpleGUI::sliderSize + SimpleGUI::padding*2.0f).y)
    );

    ci::gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);

    ci::gl::color(SimpleGUI::darkColor);
    ci::gl::drawSolidRect(activeArea);

    ci::gl::color(SimpleGUI::lightColor);
    ci::gl::drawSolidRect(SimpleGUI::getScaledWidthRectf(activeArea, getNormalizedValue()));

    pos.y += SimpleGUI::labelSize.y + SimpleGUI::padding.y + SimpleGUI::sliderSize.y + SimpleGUI::spacing;
    return pos;
}

std::string FloatVarControl::toString() {
    std::stringstream ss;
    ss << *var;
    return ss.str();
}

void FloatVarControl::fromString(std::string& strValue) {
    *var = boost::lexical_cast<float>(strValue);
}

void FloatVarControl::onMouseDown(ci::app::MouseEvent event) {
    onMouseDrag(event);
}

void FloatVarControl::onMouseDrag(ci::app::MouseEvent event) {
    float value = (event.getPos().x - activeArea.x1)/(activeArea.x2 - activeArea.x1);
    value = ci::math<float>::max(0.0, ci::math<float>::min(value, 1.0));
    setNormalizedValue(value);
}

//-----------------------------------------------------------------------------

IntVarControl::IntVarControl(const std::string& name, int* var, int min, int max, int defaultValue) {
    this->type = Control::INT_VAR;
    this->name = name;
    this->var = var;
    this->min = min;
    this->max = max;
    if (defaultValue < min) {
        *var = min;
    }
    else if (defaultValue > max) {
        *var = max;
    }
    else {
        *var = defaultValue;
    }
}

float IntVarControl::getNormalizedValue() {
    return (*var - min)/(float)(max - min);
}

void IntVarControl::setNormalizedValue(float value) {
    int newValue = min + value*(max - min);
    if (newValue != *var) {
        *var = newValue;
    }
}

ci::vec2 IntVarControl::draw(ci::vec2 pos) {
    activeArea = ci::Rectf(
                       pos.x,
                       pos.y + SimpleGUI::labelSize.y + SimpleGUI::padding.y,
                       pos.x + SimpleGUI::sliderSize.x,
                       pos.y + SimpleGUI::labelSize.y + SimpleGUI::padding.y + SimpleGUI::sliderSize.y
                      );

    ci::gl::color(SimpleGUI::bgColor);
    ci::gl::drawSolidRect(ci::Rectf(
                            (pos - SimpleGUI::padding).x,
                            (pos - SimpleGUI::padding).y,
                            (pos + SimpleGUI::sliderSize + SimpleGUI::padding).x,
                            (pos + SimpleGUI::labelSize + SimpleGUI::sliderSize + SimpleGUI::padding*2.0f).y)
                     );

    ci::gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);

    ci::gl::color(SimpleGUI::darkColor);
    ci::gl::drawSolidRect(activeArea);

    ci::gl::color(SimpleGUI::lightColor);
    ci::gl::drawSolidRect(SimpleGUI::getScaledWidthRectf(activeArea, getNormalizedValue()));

    pos.y += SimpleGUI::labelSize.y + SimpleGUI::padding.y + SimpleGUI::sliderSize.y + SimpleGUI::spacing;
    return pos;
}

std::string IntVarControl::toString() {
    std::stringstream ss;
    ss << *var;
    return ss.str();
}

void IntVarControl::fromString(std::string& strValue) {
    *var = boost::lexical_cast<int>(strValue);
}

    void IntVarControl::onMouseDown(ci::app::MouseEvent event) {
    onMouseDrag(event);
}

    void IntVarControl::onMouseDrag(ci::app::MouseEvent event) {
        float value = (event.getPos().x - activeArea.x1)/(activeArea.x2 - activeArea.x1);
        value = ci::math<float>::max(0.0, ci::math<float>::min(value, 1.0));
        setNormalizedValue(value);
}

//-----------------------------------------------------------------------------

BoolVarControl::BoolVarControl(const std::string& name, bool* var, bool defaultValue, int groupId) {
    this->type = Control::BOOL_VAR;
    this->name = name;
    this->var = var;
    this->groupId = groupId;
    *var = defaultValue;
}

ci::vec2 BoolVarControl::draw(ci::vec2 pos) {
    activeArea = ci::Rectf(pos.x, pos.y, pos.x + SimpleGUI::sliderSize.y, pos.y + SimpleGUI::sliderSize.y);
    ci::gl::color(SimpleGUI::bgColor);
    ci::gl::drawSolidRect(ci::Rectf(
        (pos - SimpleGUI::padding).x,
        (pos - SimpleGUI::padding).y,
        (pos + SimpleGUI::sliderSize + SimpleGUI::padding).x,
        (pos + SimpleGUI::sliderSize + SimpleGUI::padding).y)
    );
    ci::gl::drawString(name, ci::vec2(pos.x + SimpleGUI::sliderSize.y + SimpleGUI::padding.x*2.0f, pos.y), SimpleGUI::textColor, SimpleGUI::textFont);
    ci::gl::color((*var) ? SimpleGUI::lightColor : SimpleGUI::darkColor);
    ci::gl::drawSolidRect(activeArea);
    pos.y += SimpleGUI::sliderSize.y + SimpleGUI::spacing;
    return pos;
}

std::string BoolVarControl::toString() {
    std::stringstream ss;
    ss << *var;
    return ss.str();
}

void BoolVarControl::fromString(std::string& strValue) {
    int value = boost::lexical_cast<int>(strValue);
    *var = value ? true : false;
}

    void BoolVarControl::onMouseDown(ci::app::MouseEvent event) {
    if (groupId > -1) {
        for (std::vector<Control*>::iterator it2 = parentGui->getControls().begin(); it2 != parentGui->getControls().end(); it2++) {
            if (((*it2)->type == Control::BOOL_VAR) && (((BoolVarControl*)(*it2))->groupId == this->groupId)) {
                *((BoolVarControl*)(*it2))->var = (*it2 == this);
            }
        }
    }
    else {
        *this->var = !*this->var;
    }
}

//-----------------------------------------------------------------------------

ColorVarControl::ColorVarControl(const std::string& name, ci::ColorA* var, ci::ColorA defaultValue, int colorModel) {
    this->type = Control::COLOR_VAR;
    this->name = name;
    this->var = var;
    this->colorModel = colorModel;
    *var = defaultValue;
    activeTrack = 0;
}

ci::vec2 ColorVarControl::draw(ci::vec2 pos) {
    activeArea1 = ci::Rectf(
        pos.x,
        pos.y + SimpleGUI::labelSize.y + SimpleGUI::padding.y,
        pos.x + SimpleGUI::sliderSize.x,
        pos.y + SimpleGUI::sliderSize.y + SimpleGUI::labelSize.y + SimpleGUI::padding.y
    );

    activeArea2 = ci::Rectf(
        pos.x,
        pos.y + SimpleGUI::labelSize.y + SimpleGUI::sliderSize.y + SimpleGUI::padding.y*2.0f,
        pos.x + SimpleGUI::sliderSize.x,
        pos.y + SimpleGUI::sliderSize.y*2.0f + SimpleGUI::padding.y*2.0f + SimpleGUI::labelSize.y
    );

    activeArea3 = ci::Rectf(
        pos.x,
        pos.y + SimpleGUI::labelSize.y + SimpleGUI::sliderSize.y*2.0f + SimpleGUI::padding.y*3.0f,
        pos.x + SimpleGUI::sliderSize.x,
        pos.y + SimpleGUI::sliderSize.y*3.0f + SimpleGUI::padding.y*3.0f + SimpleGUI::labelSize.y
    );

    activeArea4 = ci::Rectf(
        pos.x,
        pos.y + SimpleGUI::labelSize.y + SimpleGUI::sliderSize.y*3.0f + SimpleGUI::padding.y*4.0f,
        pos.x + SimpleGUI::sliderSize.x,
        pos.y + SimpleGUI::sliderSize.y*4.0f + SimpleGUI::padding.y*4.0f + SimpleGUI::labelSize.y
    );

    activeArea = ci::Rectf(
        pos.x,
        pos.y + SimpleGUI::labelSize.y,
        pos.x + SimpleGUI::sliderSize.x,
        pos.y + SimpleGUI::sliderSize.y*4.0f + SimpleGUI::padding.y*4.0f + SimpleGUI::labelSize.y
    );

    ci::vec4 values;
    if (colorModel == SimpleGUI::RGB) {
        values = ci::vec4(var->r, var->g, var->b, var->a);
    }
    else {
        ci::vec3 hsv = rgbToHsv(*var);
        values.x = hsv.x;
        values.y = hsv.y;
        values.z = hsv.z;
        values.w = var->a;
    }

    ci::gl::color(SimpleGUI::bgColor);
    ci::gl::drawSolidRect(ci::Rectf(
        (pos - SimpleGUI::padding).x,
        (pos - SimpleGUI::padding).y,
        (pos + SimpleGUI::sliderSize + SimpleGUI::padding).x,
        (pos + SimpleGUI::labelSize + SimpleGUI::sliderSize*4.0f + SimpleGUI::padding*5.0f).y)
    );

    ci::gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);
    ci::gl::color(SimpleGUI::darkColor);
    ci::gl::drawSolidRect(activeArea1);
    ci::gl::drawSolidRect(activeArea2);
    ci::gl::drawSolidRect(activeArea3);
    ci::gl::drawSolidRect(activeArea4);
    ci::gl::color(SimpleGUI::lightColor);
    ci::Rectf rect1 = SimpleGUI::getScaledWidthRectf(activeArea1, values.x);
    ci::Rectf rect2 = SimpleGUI::getScaledWidthRectf(activeArea2, values.y);
    ci::Rectf rect3 = SimpleGUI::getScaledWidthRectf(activeArea3, values.z);
    ci::Rectf rect4 = SimpleGUI::getScaledWidthRectf(activeArea4, values.w);
    ci::gl::drawLine(ci::vec2(rect1.x2, rect1.y1), ci::vec2(rect1.x2, rect1.y2));
    ci::gl::drawLine(ci::vec2(rect2.x2, rect2.y1), ci::vec2(rect2.x2, rect2.y2));
    ci::gl::drawLine(ci::vec2(rect3.x2, rect3.y1), ci::vec2(rect3.x2, rect3.y2));
    ci::gl::drawLine(ci::vec2(rect4.x2, rect4.y1), ci::vec2(rect4.x2, rect4.y2));
    pos.y += SimpleGUI::labelSize.y + SimpleGUI::padding.y + SimpleGUI::sliderSize.y * 4.0f + SimpleGUI::padding.y * 3.0f + SimpleGUI::spacing;
    return pos;
}

std::string ColorVarControl::toString() {
    std::stringstream ss;
    ss << var->r << " " << var->g << " " << var->b << " " << var->a;
    return ss.str();
}

void ColorVarControl::fromString(std::string& strValue) {
    std::vector<std::string> strs;
    boost::split(strs, strValue, boost::is_any_of("\t "));
    var->r = boost::lexical_cast<double>(strs[0]);
    var->g = boost::lexical_cast<double>(strs[1]);
    var->b = boost::lexical_cast<double>(strs[2]);
    var->a = boost::lexical_cast<double>(strs[3]);
}


    void ColorVarControl::onMouseDown(ci::app::MouseEvent event) {
    if (activeArea1.contains(event.getPos())) {
        activeTrack = 1;
    }
    else if (activeArea2.contains(event.getPos())) {
        activeTrack = 2;
    }
    else if (activeArea3.contains(event.getPos())) {
        activeTrack = 3;
    }
    else if (activeArea4.contains(event.getPos())) {
        activeTrack = 4;
    }
    onMouseDrag(event);
}

    void ColorVarControl::onMouseDrag(ci::app::MouseEvent event) {
    float value = (event.getPos().x - activeArea.x1)/(activeArea.x2 - activeArea.x1);
        value = ci::math<float>::max(0.0, ci::math<float>::min(value, 1.0));

    if (colorModel == SimpleGUI::RGB) {
        switch (activeTrack) {
            case 1: var->r = value; break;
            case 2: var->g = value; break;
            case 3: var->b = value; break;
            case 4: var->a = value; break;
        }
    }
    else {
        ci::vec3 hsv = rgbToHsv(*var);
        switch (activeTrack) {
            case 1: hsv.x = value; break;
            case 2: hsv.y = value; break;
            case 3: hsv.z = value; break;
            case 4: var->a = value; break;
        }
        *var = ci::ColorA(ci::CM_HSV, hsv.x, hsv.y, hsv.z, var->a);
    }
}

//-----------------------------------------------------------------------------

ButtonControl::ButtonControl(const std::string& name) {
    this->type = Control::BUTTON;
    this->name = name;
    this->pressed = false;
}

ci::vec2 ButtonControl::draw(ci::vec2 pos) {
    activeArea = ci::Rectf(
                       pos.x,
                       pos.y,
                       pos.x + SimpleGUI::sliderSize.x,
                       pos.y + SimpleGUI::labelSize.y + SimpleGUI::padding.y*1.0f
                      );

    ci::gl::color(SimpleGUI::bgColor);
    ci::gl::drawSolidRect(ci::Rectf(
                            (pos - SimpleGUI::padding).x,
                            (pos - SimpleGUI::padding).y,
                            (pos + SimpleGUI::sliderSize + SimpleGUI::padding).x,
                            (pos + SimpleGUI::sliderSize + SimpleGUI::padding * 2.0f).y)
                     );


    ci::gl::color(pressed ? SimpleGUI::lightColor : SimpleGUI::darkColor);
    ci::gl::drawSolidRect(activeArea);
    ci::gl::drawString(name, ci::vec2(pos.x + SimpleGUI::padding.x * 2.0f, pos.y + floor(SimpleGUI::padding.y * 0.5f)), pressed ? SimpleGUI::darkColor : SimpleGUI::textColor, SimpleGUI::textFont);
    pos.y += SimpleGUI::sliderSize.y + SimpleGUI::spacing + SimpleGUI::padding.y;
    return pos;
}

    void ButtonControl::onMouseDown(ci::app::MouseEvent event) {
    pressed = true;
    fireClick();
}

    void ButtonControl::onMouseUp(ci::app::MouseEvent event) {
    pressed = false;
}

void ButtonControl::fireClick() {
    ci::app::MouseEvent event;
    bool handled = false;
    for (ci::CallbackMgr<bool(ci::app::MouseEvent)>::iterator cbIter = callbacksClick.begin(); (cbIter != callbacksClick.end()) && (!handled); ++cbIter) {
        handled = (cbIter->second)(event);
    }
}

//-----------------------------------------------------------------------------

LabelControl::LabelControl(const std::string& name) {
    this->type = Control::LABEL;
    this->name = name;
}

void LabelControl::setText(const std::string& text) {
    name = text;
}

ci::vec2 LabelControl::draw(ci::vec2 pos) {
    if (bgColor == ci::ColorA(0.0, 0.0, 0.0, 0.5)) {
        ci::gl::color(bgColor);
    }
    else {
        ci::gl::color(SimpleGUI::bgColor);
    }
    ci::gl::drawSolidRect(ci::Rectf(
        (pos - SimpleGUI::padding).x,
        (pos - SimpleGUI::padding).y,
        (pos + SimpleGUI::sliderSize + SimpleGUI::padding).x,
        (pos + SimpleGUI::labelSize + SimpleGUI::padding).y
    ));
    ci::gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);
    pos.y += SimpleGUI::labelSize.y + SimpleGUI::spacing;
    return pos;
}

//-----------------------------------------------------------------------------

SeparatorControl::SeparatorControl() {
    this->type = Control::SEPARATOR;
    this->name = "Separator";
}

ci::vec2 SeparatorControl::draw(ci::vec2 pos) {
    activeArea = ci::Rectf(pos - SimpleGUI::padding, pos + SimpleGUI::separatorSize + SimpleGUI::padding);
    ci::gl::color(SimpleGUI::bgColor);
    ci::gl::drawSolidRect(activeArea);
    pos.y += SimpleGUI::separatorSize.y + SimpleGUI::spacing;
    return pos;
}

//-----------------------------------------------------------------------------

ColumnControl::ColumnControl(int x, int y) {
    this->x = x;
    this->y = y;
    this->type = Control::COLUMN;
    this->name = "Column";
}

ci::vec2 ColumnControl::draw(ci::vec2 pos) {
    if (this->x == 0 && this->y == 0) {
        pos.x += SimpleGUI::labelSize.x + SimpleGUI::spacing;
        pos.y = SimpleGUI::spacing;
    }
    else {
        pos.x = this->x;
        pos.y = this->y;
    }
    return pos;
}

//-----------------------------------------------------------------------------

PanelControl::PanelControl() {
    this->enabled = true;
    this->type = Control::PANEL;
    this->name = "Panel";
}

ci::vec2 PanelControl::draw(ci::vec2 pos) {
    return pos;
}

//-----------------------------------------------------------------------------

TextureVarControl::TextureVarControl(const std::string& name, ci::gl::Texture* var, int scale, bool flipVert) {
    this->type = Control::TEXTURE_VAR;
    this->name = name;
    this->var = var;
    this->scale = scale;
    this->flipVert = flipVert;
}

ci::vec2 TextureVarControl::draw(ci::vec2 pos) {
    if (!var) {
        std::cout << "Missing texture" << std::endl;
        return pos;
    }

    ci::vec2 textureSize;
    textureSize.x = ci::math<float>::floor(SimpleGUI::sliderSize.x * scale + SimpleGUI::spacing * (scale - 1));
    textureSize.y = ci::math<float>::floor(textureSize.x / var->getAspectRatio());
    activeArea = ci::Rectf(pos.x, pos.y, pos.x + textureSize.x, pos.y + textureSize.y);
    ci::gl::color(SimpleGUI::bgColor);
    ci::gl::drawSolidRect(ci::Rectf(
        activeArea.x1 - SimpleGUI::padding.x,
        activeArea.y1 - SimpleGUI::padding.y,
        activeArea.x2 + SimpleGUI::padding.x,
        activeArea.y2 + SimpleGUI::padding.y
    ));
    ci::gl::color(1, 1, 1, 1);
    ci::gl::draw((ci::gl::TextureRef)var, activeArea);
    pos.y += activeArea.getHeight() + SimpleGUI::spacing;
    return pos;
}

//-----------------------------------------------------------------------------

}  // namespace sgui
}  // namespace mowa
