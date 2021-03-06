/*                                                         -*- c++ -*-
 * Copyright (C) 2006, 2007 Andreas Persson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with program; see the file COPYING. If not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#ifndef GIGEDIT_REGIONCHOOSER_H
#define GIGEDIT_REGIONCHOOSER_H

#include <gtkmm/drawingarea.h>
#include <gdkmm/colormap.h>
#include <gdkmm/window.h>

#include <gig.h>

class RegionChooser : public Gtk::DrawingArea
{
public:
    RegionChooser();
    virtual ~RegionChooser();

    void set_instrument(gig::Instrument* instrument);
    void set_region(gig::Region* region);

    sigc::signal<void> signal_sel_changed();

    gig::Region* get_region() { return region; }

protected:
    virtual void on_realize();
    virtual bool on_expose_event(GdkEventExpose* e);
    virtual void on_size_request(GtkRequisition* requisition);
    virtual bool on_button_press_event(GdkEventButton* event);
    virtual bool on_motion_notify_event(GdkEventMotion* event);

//    virtual void on_size_allocate(Gtk::Allocation& allocation);

    Glib::RefPtr<Gdk::GC> gc;
    Gdk::Color blue, red, black, white, green, grey1;

    void draw_region(int from, int to, const Gdk::Color& color);

    sigc::signal<void> sel_changed_signal;

    gig::Instrument* instrument;
    gig::Region* region;
};

#endif
