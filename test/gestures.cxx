#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Dial.H>

#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const float zmin = 0.4; // min. zoom
const float zmax = 2.0; // max. zoom

void exitcb(Fl_Widget *, void *) {
  exit(0);
}

class myButton : public Fl_Button {
  char txt_[20];
  double zoom;
  int bx, by, bw, bh;

public:
  myButton(int x, int y, int w, int h): Fl_Button(x, y, w, h, "") {
    bx = x; by = y; bw = w; bh = h;
    zoom = 1.0;
    strcpy(txt_, "1.0");
    align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
    label(txt_);
  }

  int handle(int event) {
    int ret = 0;
    double z = 0.0;
    switch(event) {
      case FL_MOUSEWHEEL:
      case FL_SCROLL_GESTURE:
        if (Fl::event_dy() > 0)
          zoom += 0.1;
        else
          zoom -= 0.1;
        if (zoom < zmin) zoom = zmin;
        if (zoom > zmax) zoom = zmax;
        sprintf(txt_, "%5.3f", zoom);
        resize(bx, by, bw * zoom, bh * zoom);
        parent()->redraw();
        ret = 1;
        break;
      case FL_ZOOM_GESTURE:
        z = Fl::event_dy()/100000.0;
        printf("handle FL_ZOOM_GESTURE: %d (%5.3f)\n", Fl::event_dy(), z);
        if (z > 0)
          zoom *= z;
        if (zoom < zmin) zoom = zmin;
        if (zoom > zmax) zoom = zmax;
        sprintf(txt_, "%5.3f", zoom);
        resize(bx, by, bw * zoom, bh * zoom);
        parent()->redraw();
        ret = 1;
        break;
      default:
        break;
    }
    return Fl_Button::handle(event) ? 1 : ret;
  }
};


class myDial : public Fl_Dial {
  char txt_[20];
  double angle;
  int bx, by, bw, bh;

public:
  myDial(int x, int y, int w, int h): Fl_Dial(x, y, w, h, "") {
    bx = x; by = y; bw = w; bh = h;
    angle = 90.0;
    strcpy(txt_, "90.0");
    align(FL_ALIGN_CENTER | FL_ALIGN_TOP);
    label(txt_);
    angles(0, 360);
    type(FL_LINE_DIAL);
    minimum(0);
    maximum(360);
    step(1);
    set_value();
  }

  void set_value() {
    value(double(int(angle + 180) % 360));
  }

  void set_label() {
    angle = (int(value()) + 180) % 360;
    sprintf(txt_, "%3.0f", angle);
    parent()->redraw();
  }

  int handle(int event) {
    int ret = 0;
    switch(event) {
      case FL_MOUSEWHEEL:
      case FL_SCROLL_GESTURE:
        if (Fl::event_dy() > 0)
          angle += 10.0;
        else
          angle -= 10.;
        if (angle < 0.) angle += 360.;
        if (angle > 360.) angle -= 360;
        sprintf(txt_, "%3.0f", angle);
        set_value();
        parent()->redraw();
        return 1;
        break;
      case FL_ROTATE_GESTURE:
        angle += Fl::event_dy() / 100000.;
        if (angle < 0.) angle += 360.;
        if (angle > 360.) angle -= 360;
        sprintf(txt_, "%3.0f", angle);
        set_value();
        parent()->redraw();
        return 1;
        break;
      default:
        break;
    }
    return Fl_Dial::handle(event) ? 1 : ret;
  }
};

void dial_cb(Fl_Widget *w, void *v) {
  myDial *d = (myDial *)w;
  d->set_label();
}

int main(int argc, char **argv) {

  Fl_Double_Window *window = new Fl_Double_Window(880, 510);

  // embed widgets in their own groups to speed up drawing

  Fl_Group *g1 = new Fl_Group(0, 0, 440, 440);
  g1->box(FL_FLAT_BOX);
  g1->color(0xddffdd00); // light green
  myButton *b1 = new myButton(20, 20, 200, 100);
  myDial *d1 = new myDial(130, 240, 180, 180);
  d1->callback(dial_cb);
  g1->end();

  Fl_Group *g2 = new Fl_Group(440, 0, 440, 440);
  g2->box(FL_FLAT_BOX);
  g2->color(0xddddff00); // light blue
  myDial *d2 = new myDial(460, 30, 380, 380);
  d2->callback(dial_cb);
  g2->end();

  Fl_Button *b3 = new Fl_Button(10, 450, 860, 50, "E&xit");
  b3->callback(exitcb, 0);

  window->end();
  window->resizable(window);
  window->size_range(430, 240);
  window->show(argc, argv);

  return Fl::run();
}
