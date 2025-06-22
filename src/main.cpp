#include <visage/app.h>


class MyApp : public visage::ApplicationWindow {
public:
    MyApp() {
            bloom_.setBloomSize(40.0f);
    setPostEffect(&bloom_);
        setTitle("Visage Toggle Circle Color");
        addChild(&frame);
        frame.layout().setHeight(100);
        frame.layout().setWidth(100);
        frame.onDraw() = [this](visage::Canvas& canvas){
            canvas.setColor(0xff888888);
            canvas.roundedRectangle(0, 0, frame.width(), frame.height(), 16);

            float hdr_value = 1.0f; // Default HDR value

            visage::Color myColor(1.0f, 0.0f, 0.0f, 1.0f, hdr_value); // R G B A (HDR)
            canvas.setColor(myColor);
            canvas.circle(frame.width()*0.25,frame.height()*0.25,50);
        };
        setPalette(&palette_);
        // Drawing code inside the constructor (or you could make a separate setup method)
        onDraw() = [this](visage::Canvas& canvas) {
    visage::Brush rainbow = visage::Brush::horizontal(visage::Gradient(0xffff6666, 0xffffff66,
                                                                       0xff66ff66, 0xff66ffff, 0xff6666ff,
                                                                       0xffff66ff, 0xffff6666));
            canvas.setColor(0xff000000); // Purple background
            canvas.fill(0, 0, width(), height());

            float circle_radius = height() * 0.25f;
            float x = width() * 0.5f - circle_radius;
            float y = height() * 0.5f - circle_radius;
            visage::Color myColor(1.0f, 0.0f, 0.0f, 1.0f, (mouse_x / this->width())*10.0f); // R G B A (HDR)
            canvas.setColor(rainbow); // Orange circle
            canvas.circle(x, y, 2.0f * circle_radius);
        };
    }

    void setPosition(const visage::Point& point) {
        mouse_x = point.x;
        mouse_y = point.y;
        //bloom_.setBloomSize(mouse_x);
        float bloom = (mouse_y / this->width())*10.0f;
        bloom_.setBloomIntensity(bloom);
        redraw();
        frame.redraw();
    }
    void mouseMove(const visage::MouseEvent &e) override {
        setPosition(e.position);
    }
    void launch() {
        show(800, 600);
        runEventLoop();
    }
private:
Frame frame;
float mouse_x = 0.0f;
float mouse_y = 0.0f;
visage::BloomPostEffect bloom_;
visage::Palette palette_;

};

int main() {
    MyApp app;
    app.launch();
    return 0;
}