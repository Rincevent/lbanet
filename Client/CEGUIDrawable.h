/*
------------------------[ Lbanet Source ]-------------------------
Copyright (C) 2009
Author: Vivien Delage [Rincevent_123]
Email : vdelage@gmail.com

-------------------------------[ GNU License ]-------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

-----------------------------------------------------------------------------
*/
#ifndef _LBANET_CEGUI_DRAWABLE_H_
#define _LBANET_CEGUI_DRAWABLE_H_

#include <osg/Drawable>
#include <boost/shared_ptr.hpp>

class GuiHandler;

enum XtGLw_state {XtGLw_Off, XtGLw_Text, XtGLw_Image};
enum XtGLw_fadingstate {XtGLw_FDOff, XtGLw_FDIn, XtGLw_FDOut};

class FONT_FONT;



//*************************************************************************************************
//*                               class CEGUIDrawable
//*************************************************************************************************
/**
* @brief Mix CEGUI with OSG
*
*/
class CEGUIDrawable : public osg::Drawable
{
public:
	//! constructor
    CEGUIDrawable();

	//! constructor
    CEGUIDrawable(int resX, int resY, boost::shared_ptr<GuiHandler> GuiH);

    /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
    CEGUIDrawable(const CEGUIDrawable& drawable,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
					: Drawable(drawable,copyop) {}
    
    META_Object(osg,CEGUIDrawable);
    

    void drawImplementation(osg::RenderInfo& renderInfo) const;



	void paintXtraGL() const;

	void PressedSpace();

	void Process(double tnow, float tdiff);


	void StartScrollingText(const std::string & imagepath, std::vector<std::vector<unsigned int> > &textIds);


	void StartFixedImage(const std::string & imagepath, long NbSecondDisplay, 
								bool FadeIn, float FadeInColorR, float FadeInColorG, float FadeInColorB,
								bool FadeOut, float FadeOutColorR, float FadeOutColorG, float FadeOutColorB);


	void EndDrawExtraGL();


	void resizedGL(int w, int h);

protected:
		
	void write_line_black(const std::vector<unsigned int> &text, FONT_FONT &font, 
								double x, double y, double space_size, int nbchar) const;

	void write_line_white(const std::vector<unsigned int> &text, FONT_FONT &font, 
								double x, double y, double space_size, int nbchar) const;

	void write_text_white(const std::vector<unsigned int> &text, FONT_FONT &font, 
								double x, double y, double maxlenght, int maxchar) const;

	void write_text_black(const std::vector<unsigned int> &text, FONT_FONT &font, 
								double x, double y, double maxlenght, int maxchar) const;



	void CreateLBAFont(int size);

	void DeleteFont();

	// load image to texture
	void LoadGLTextures(const std::string& name);

	void CleanImageTexture();


	// clean up
	void CleanUp();

	// clean up display and report terminaison
	void CleanAndReport();

	//! display image if exist
	void DrawBGImage(float alpha) const;


protected:    
	//! destructor
    virtual ~CEGUIDrawable();

    unsigned int _activeContextID;

private:
	boost::shared_ptr<GuiHandler>		_GuiH;

	bool								_drawXtraGL;



	float			_bgR;
	float			_bgG;
	float			_bgB;
	float			_bgA;

	bool			_fontloaded;
	FONT_FONT *		_loadedfont;
	GLuint			_textureid;

	bool			_imageloaded;
	GLuint			_imgtextureid;

	int				_windowW;
	int				_windowH;

	XtGLw_state			_currentstate;
	XtGLw_fadingstate	_currentfadestate;

	bool			_scrolling;
	float			_scrollingtimediff;
	std::vector<std::vector<unsigned int> >	_texts;
	int							_textidx;

	double			_textfinishdisplaytime;
	bool			_fadingin;
	bool			_fadingout;
	float			_fiR;
	float			_fiG;
	float			_fiB;
	float			_foR;
	float			_foG;
	float			_foB;
	float			_currentalpha;
};


#endif