pixelpunch
==========

Pixelpunch let's you rotate, sheer and scale pixel graphics and tweak the result by picking from a wide range of different upscaling, transformation and sampling algorithms.

![image](https://cloud.githubusercontent.com/assets/1045397/22601581/da226036-ea3f-11e6-842e-13f1f9991d22.png)

The project was discontinued by Lithander.
This fork has updated the source code to Cinder 0.9 and intend add more features.

## How to use
Drag and drop an image into the program or use the open button.
Hold down Right mouse button to rotate when you're in projection or billinear transformation mode   
Hold shift to lock rotation to certain angles.

### Shortcuts
Open: **O**   
Save: **S**   
Reset: **R**   

Notice:   
if you save without specifying an extension ie. PNG, JPEG etc. then the image will be saved as BMP.
 
It's C++ code and requires the Cinder 0.9 Framework to run.  
Windows:  
https://libcinder.org/static/releases/cinder_0.9.0_vc2013.zip

Mac:  
https://libcinder.org/static/releases/cinder_0.9.0_mac.zip

The easiest way to compile it is to treat it as another of Cinder's many sample apps.
Here's what you need to do if you happen to use VS 2013.

* In "cinder\samples" add another folder called "PixelPunch"
* Copy the sourcecode there.
* Open "AllSamples.sln"
* Add Existing Project and chose "cinder\samples\PixelPunch\vc11\PixelPunch.vcxproj"
* Compile & Profit

Here's what to do if you use Xcode 8.   
* In "cinder/samples" add another folder called PixelPunch
* Copy the sourcecode there.
* Open AllSamples in cinder/samples/_AllSamples/xcode/
* Drag the Pixelpunch.xcodeproj into the Project navigator
* Select PixelPunch as the active scheme
* Compile

References:

http://wayofthepixel.net/index.php?topic=12502.0
http://www.alonsomartin.mx/hfa/2013/10/30/spriting-tips/
