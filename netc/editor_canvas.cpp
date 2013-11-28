#include "image_mgr.h"
#include "editor_canvas.h"
#include "card_data.h"
#include "deck_data.h"
#include "image_mgr.h"

namespace ygopro
{
	BEGIN_EVENT_TABLE(wxEditorCanvas, wxGLCanvas)
		EVT_SIZE(wxEditorCanvas::eventResized)
		EVT_PAINT(wxEditorCanvas::eventRender)
		EVT_MOTION(wxEditorCanvas::eventMouseMoved)
		EVT_MOUSEWHEEL(wxEditorCanvas::eventMouseWheelMoved)
		EVT_LEFT_DOWN(wxEditorCanvas::eventMouseDown)
		EVT_LEFT_UP(wxEditorCanvas::eventMouseReleased)
		EVT_LEAVE_WINDOW(wxEditorCanvas::eventMouseLeftWindow)
	END_EVENT_TABLE()

	wxEditorCanvas::wxEditorCanvas(wxFrame* parent, int id, int* args): wxGLCanvas(parent, id, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE) {
        glcontext = new wxGLContext(this);
        SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        t_buildbg = &imageMgr.textures["buildbg"];
        t_limits[0] = &imageMgr.textures["limit0"];
        t_limits[1] = &imageMgr.textures["limit1"];
        t_limits[2] = &imageMgr.textures["limit2"];
	}

	wxEditorCanvas::~wxEditorCanvas() {
		delete glcontext;
	}

	void wxEditorCanvas::eventResized(wxSizeEvent& evt) {
		glwidth = evt.GetSize().GetWidth();
		glheight = evt.GetSize().GetHeight();
		glViewport(0, 0, glwidth, glheight);
		Refresh();
	}

	void wxEditorCanvas::eventRender(wxPaintEvent& evt) {
		if(!IsShown())
			return;
		wxGLCanvas::SetCurrent(*glcontext);
		wxPaintDC(this);
        imageMgr.InitTextures();
		drawScene();
		SwapBuffers();
	}

	void wxEditorCanvas::eventMouseMoved(wxMouseEvent& evt){

	}

	void wxEditorCanvas::eventMouseWheelMoved(wxMouseEvent& evt){

	}

	void wxEditorCanvas::eventMouseDown(wxMouseEvent& evt){

	}

	void wxEditorCanvas::eventMouseReleased(wxMouseEvent& evt){

	}

	void wxEditorCanvas::eventMouseLeftWindow(wxMouseEvent& evt){

	}
    
	void wxEditorCanvas::drawScene() {

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glShadeModel(GL_SMOOTH);

		//draw background
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, t_buildbg->tex());
		glBegin(GL_QUADS);
		{
			glTexCoord2f(t_buildbg->lx, t_buildbg->ly);glVertex2f(-1.0f, 1.0f);
			glTexCoord2f(t_buildbg->lx, t_buildbg->ry);glVertex2f(-1.0f, -1.0f);
			glTexCoord2f(t_buildbg->rx, t_buildbg->ry);glVertex2f(1.0f, -1.0f);
			glTexCoord2f(t_buildbg->rx, t_buildbg->ly);glVertex2f(1.0f, 1.0f);
		}
        glEnd();
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
        size_t main_size = current_deck.main_deck.size();
        float wd = 0.2f * glheight / glwidth;
        float ht = 0.29f;
        float iconw = 0.08f * glheight / glwidth;
        float iconh = 0.08f;
        size_t line_size = 18.0f / 11 / wd;
        if(main_size > line_size * 4)
            line_size = 10 + (main_size - 40) / 4;
        if(line_size < 10)
            line_size = 10;
        float sx = -0.85f, sy = 0.90f;
        float dx = (1.8f - wd) / (line_size - 1);
        if(dx > wd * 11.0f / 10.0f)
            dx = wd * 11.0f / 10.0f;
        float dy = 0.3f;
        for(size_t i = 0; i < main_size; ++i) {
            TextureInfo* ti = std::get<1>(current_deck.main_deck[i]);
            int limit = std::get<2>(current_deck.main_deck[i]);
            if(ti == nullptr) {
                ti = &imageMgr.GetCardTexture(std::get<0>(current_deck.main_deck[i])->code);
                std::get<1>(current_deck.main_deck[i]) = ti;
            }
            size_t lx = i % line_size;
            size_t ly = i / line_size;
            glBindTexture(GL_TEXTURE_2D, ti->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(ti->lx, ti->ly);glVertex2f(sx + lx * dx, sy - ly * dy);
                glTexCoord2f(ti->lx, ti->ry);glVertex2f(sx + lx * dx, sy - ly * dy - ht);
                glTexCoord2f(ti->rx, ti->ry);glVertex2f(sx + lx * dx + wd, sy - ly * dy - ht);
                glTexCoord2f(ti->rx, ti->ly);glVertex2f(sx + lx * dx + wd, sy - ly * dy);
            }
            glEnd();
            if(limit >= 3)
                continue;
            glBindTexture(GL_TEXTURE_2D, t_limits[limit]->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ly);glVertex2f(sx - 0.01f + lx * dx, sy + 0.01f - ly * dy);
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ry);glVertex2f(sx - 0.01f + lx * dx, sy + 0.01f - ly * dy - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ry);glVertex2f(sx - 0.01f + lx * dx + iconw, sy + 0.01f - ly * dy - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ly);glVertex2f(sx - 0.01f + lx * dx + iconw, sy + 0.01f - ly * dy);
            }
            glEnd();
        }
        size_t extra_size = current_deck.extra_deck.size();
        sx = -0.85f;
        sy = -0.32f;
        dx = (1.8f - wd) / (extra_size - 1);
        if(dx > wd * 11.0f / 10.0f)
            dx = wd * 11.0f / 10.0f;
        for(size_t i = 0; i < extra_size; ++i) {
            TextureInfo* ti = std::get<1>(current_deck.extra_deck[i]);
            int limit = std::get<2>(current_deck.extra_deck[i]);
            if(ti == nullptr) {
                ti = &imageMgr.GetCardTexture(std::get<0>(current_deck.extra_deck[i])->code);
                std::get<1>(current_deck.extra_deck[i]) = ti;
            }
            glBindTexture(GL_TEXTURE_2D, ti->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(ti->lx, ti->ly);glVertex2f(sx + i * dx, sy);
                glTexCoord2f(ti->lx, ti->ry);glVertex2f(sx + i * dx, sy - ht);
                glTexCoord2f(ti->rx, ti->ry);glVertex2f(sx + i * dx + wd, sy - ht);
                glTexCoord2f(ti->rx, ti->ly);glVertex2f(sx + i * dx + wd, sy);
            }
            glEnd();
            if(limit >= 3)
                continue;
            glBindTexture(GL_TEXTURE_2D, t_limits[limit]->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ly);glVertex2f(sx - 0.01f + i * dx, sy + 0.01f);
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ry);glVertex2f(sx - 0.01f + i * dx, sy + 0.01f - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ry);glVertex2f(sx - 0.01f + i * dx + iconw, sy + 0.01f - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ly);glVertex2f(sx - 0.01f + i * dx + iconw, sy + 0.01f);
            }
            glEnd();
        }
        size_t side_size = current_deck.side_deck.size();
        sx = -0.85f;
        sy = -0.64f;
        dx = (1.8f - wd) / (side_size - 1);
        if(dx > wd * 11.0f / 10.0f)
            dx = wd * 11.0f / 10.0f;
        for(size_t i = 0; i < side_size; ++i) {
            TextureInfo* ti = std::get<1>(current_deck.side_deck[i]);
            int limit = std::get<2>(current_deck.side_deck[i]);
            if(ti == nullptr) {
                ti = &imageMgr.GetCardTexture(std::get<0>(current_deck.side_deck[i])->code);
                std::get<1>(current_deck.side_deck[i]) = ti;
            }
            glBindTexture(GL_TEXTURE_2D, ti->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(ti->lx, ti->ly);glVertex2f(sx + i * dx, sy);
                glTexCoord2f(ti->lx, ti->ry);glVertex2f(sx + i * dx, sy - ht);
                glTexCoord2f(ti->rx, ti->ry);glVertex2f(sx + i * dx + wd, sy - ht);
                glTexCoord2f(ti->rx, ti->ly);glVertex2f(sx + i * dx + wd, sy);
            }
            glEnd();
            if(limit >= 3)
                continue;
            glBindTexture(GL_TEXTURE_2D, t_limits[limit]->tex());
            glBegin(GL_QUADS);
            {
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ly);glVertex2f(sx - 0.01f + i * dx, sy + 0.01f);
                glTexCoord2f(t_limits[limit]->lx, t_limits[limit]->ry);glVertex2f(sx - 0.01f + i * dx, sy + 0.01f - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ry);glVertex2f(sx - 0.01f + i * dx + iconw, sy + 0.01f - iconh);
                glTexCoord2f(t_limits[limit]->rx, t_limits[limit]->ly);glVertex2f(sx - 0.01f + i * dx + iconw, sy + 0.01f);
            }
        }
		glFlush();
	}

}
