#include "TransformUI.h"
#include "cinder/gl/gl.h"
#include "/cinder/app/App.h"
using namespace ci;
using namespace cinder::app;

TransformUI::TransformUI(void) :
    mHandleRadius(4.0f),
    mRotationHandleLength(40.0f),
    mViewScale(1.0f),
    mShapeOffset(0, 0),
    mDraggedEdge(NONE),
    mDraggedCorner(NONE),
    mLeftMouseDown(false),
    mRightMouseDown(false),
    mIsRotating(false),
    mResetPressed(false),
    mShiftDown(false)
{
}

void TransformUI::init()
{
    for (int i = 0; i < 4; i++)
        mInitRotShape[i] = shape[i];
}

TransformUI::~TransformUI(void)
{
}

void TransformUI::center()
{
    mShapeOffset = vec2(0, 0);
    for (int i = 0; i < 4; i++)
        mShapeOffset += shape[i];
    mShapeOffset /= 4;
    updateMatrix();
}

void TransformUI::resetTransformation()
{
    for (int i = 0; i < 4; i++){
        vec2 rotatePoints = rotatePoint(mInitRotShape[i], vec2(0, 0), 0);
        shape[i] = rotatePoints;
    }
    TransformUI::center();
}

void TransformUI::updateMatrix()
{
    mShapeToView = mat4(1);  // Identity matrix
    mShapeToView *= translate(vec3(0.5f * mViewSize, 0.0f));
    mShapeToView *= scale(vec3(mViewScale, mViewScale, 1.0f));
    mShapeToView *= translate(vec3(-mShapeOffset, 0.0f));

    mViewToShape = inverse(mShapeToView);
}

void TransformUI::setView(ci::ivec2 windowSize, float scale)
{
    mViewSize = windowSize;
    mViewScale = scale;
    updateMatrix();
}

void TransformUI::setShape(ci::Rectf rect)
{
    shape[TOP_LEFT] = rect.getUpperLeft();
    shape[TOP_RIGHT] = rect.getUpperRight();
    shape[BOTTOM_LEFT] = rect.getLowerLeft();
    shape[BOTTOM_RIGHT] = rect.getLowerRight();
}

ci::Rectf TransformUI::getBounds()
{
    Rectf result(shape[0], vec2());
    for (int i = 0; i < 4; i++)
    {
        result.x1 = std::min(shape[i].x, result.x1);
        result.y1 = std::min(shape[i].y, result.y1);

        result.x2 = std::max(shape[i].x, result.x2);
        result.y2 = std::max(shape[i].y, result.y2);
    }
    return result;
}

bool TransformUI::mouseMove(MouseEvent evt)
{
    vec2 pos = evt.getPos();
    vec3 transformPoint = vec3(mViewToShape* vec4(pos, 0, 1));
    vec2 posShape = vec2(transformPoint.x, transformPoint.y);
    mDraggedCorner = NONE;
    mDraggedEdge = NONE;
    mLeftMouseDown = false;
    mRightMouseDown = false;

    // Make sure we can't interact with image on GUI
    if (pos.x >= 135 && pos.x < ci::app::getWindowWidth())
    {
        // A handle to drag?
        for (int i = 0; i < 4; i++)
        {
            if (distance(shape[i], posShape) < mHandleRadius)
            {
                mDraggedCorner = i;
                return true;
            }
        }

        // An edge to drag?
        for (int i = 0; i < 4; i++)
        {
         vec2 edgeDir = normalize((shape[(i+1)%4] - shape[i]));
            vec2 cornerToMouse = posShape - shape[i];
            vec2 edgeToMouse = cornerToMouse - edgeDir * dot(cornerToMouse, edgeDir);
            if (length(edgeToMouse) < mHandleRadius)
            {
                vec2 posSnapped = posShape - edgeToMouse;
                mDraggedEdge = i;
                mEdgeStartToMouse = posSnapped - shape[i];
                mMouseToEdgeEnd = shape[(i+1)%4] - posSnapped;
                return true;
            }
        }
    }
    return false;
}

bool TransformUI::mouseDown(MouseEvent evt)
{
    mMouseDragStart = evt.getPos();
    mRightMouseDown = evt.isRightDown();
    mLeftMouseDown = evt.isLeftDown();
    return false;
}

bool TransformUI::mouseUp(MouseEvent evt)
{
    mIsRotating = false;
    mLeftMouseDown = evt.isLeftDown();
    mRightMouseDown = evt.isRightDown();
    if (!mRightMouseDown)
        mIsRotating = false;
    return false;
}

bool TransformUI::mouseDrag(MouseEvent evt)
{
    vec2 pos = evt.getPos();
    vec3 transformPoint =  vec3(mViewToShape* vec4(pos, 0, 1));
    vec2 posShape = vec2(transformPoint.x, transformPoint.y);
    if (mLeftMouseDown)
    {
        // DRAG EDGE
        if (mDraggedEdge >= 0)
        {
            shape[mDraggedEdge] = posShape - mEdgeStartToMouse;
            shape[(mDraggedEdge+1)%4] = posShape + mMouseToEdgeEnd;
            return true;
        }
        // DRAG CORNER
        if (mDraggedCorner >= 0)
        {
            shape[mDraggedCorner] = posShape;
            return true;
        }
    }
    // ROTATE SHAPE
    if (mRightMouseDown)
    {
        vec2 pos = evt.getPos();
        if (!mIsRotating)
        {
            mRotationStartDir = pos - mMouseDragStart;
            if (length(mRotationStartDir) > mRotationHandleLength)
            {
                mRotationAngle = 0;
                mIsRotating = true;
                for (int i = 0; i < 4; i++)
                    mPreRotShape[i] = shape[i];
            }
        }
        if (mIsRotating)
        {
            mRotationCurrentDir = pos - mMouseDragStart;
            mRotationAngle = getAngle(mRotationStartDir, mRotationCurrentDir);
            for (int i = 0; i < 9; i++)
            {
                // Lock to angles
                if ((mShiftDown && mRotationAngle >= lockedAngles[i]) && mRotationAngle <= lockedAngles[i+1])
                {
                    mRotationAngle = lockedAngles[i];
                    vec3 rotBase = vec3(mViewToShape* vec4(mMouseDragStart, 0, 1));
                    for (int j = 0; j < 4; j++){
                        vec2 rotatePoints = rotatePoint(mPreRotShape[j], vec2(rotBase), mRotationAngle);
                        shape[j] = rotatePoints;
                    }
                }
            }
            if (!mShiftDown && mRotationAngle > 0.01){
                vec3 rotBase = vec3(mViewToShape* vec4(mMouseDragStart, 0, 1));
                for (int i = 0; i < 4; i++){
                    vec2 rotatePoints = rotatePoint(mPreRotShape[i], vec2(rotBase), mRotationAngle);
                    shape[i] = rotatePoints;
                }
            }
            return true;
        }
    }
    return false;
}

float TransformUI::getAngle(const vec2& v1, const vec2& v2)
{
    float angle = acosf(dot(normalize(v1), normalize(v2)));

    if (cross(vec3(v2, 0), vec3(v1, 0)).z > 0.0f)
        angle = 2 * M_PI - angle;
    return angle;
}

vec2 TransformUI::rotatePoint(const vec2& point, const vec2& rotBase, float angleRadian)
{
    vec2 v = point - rotBase;
    v = rotate(v, angleRadian);
    return rotBase + v;
}

void TransformUI::draw()
{
    gl::pushMatrices();

    gl::multModelMatrix(mShapeToView);

    for (int i = 0; i < 4; i++)
    {
        Path2d path;

        gl::color(0.4f, 0.4f, 0.4f);
        // draw rect
        if (mDraggedEdge == i)
            gl::color(0.4f, 0.8f, 0.4f);
        path.moveTo(shape[i]);
        path.lineTo(shape[(i+1)%4]);
        path.close();
        gl::draw(path);
        // draw handles
        gl::color(0.4f, 0.4f, 0.4f);
        if (mDraggedCorner == i)
            gl::color(0.4f, 0.8f, 0.4f);
        gl::drawSolidCircle(shape[i], mHandleRadius/mViewScale, 30);
    }
    gl::popMatrices();
    drawRotationCone();
}

void TransformUI::drawRotationCone()
{
    // draw rotation helper
    if (mIsRotating)
    {
        Path2d path;
        path.moveTo(mMouseDragStart);
        float startAngle = getAngle(vec2(1, 0), mRotationStartDir);
        if (mRotationAngle < 0 || mRotationAngle > M_PI)
            path.arc(mMouseDragStart, length(mRotationCurrentDir), startAngle+mRotationAngle, startAngle);
        else
            path.arc(mMouseDragStart, length(mRotationCurrentDir), startAngle, startAngle+mRotationAngle);
        path.close();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_RGBA);
        gl::color(1.0f, 1.0f, 1.0f, 0.2f);
        gl::drawSolid(path);
        gl::color(0.4f, 0.8f, 0.4f);
        gl::draw(path);
        glDisable(GL_BLEND);
    }
}

