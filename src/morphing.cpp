#include <visage/app.h>
#include <vector>
#include <cmath>

// CORRECTED: Lower point count to match the target image
constexpr int MAX_POINTS = 10;
constexpr float TAU = 6.28318530718f;

/**
 * @class SplineDeformation
 * @brief A Visage Frame that draws a closed, deformed spline, matching the shader output.
 */
class SplineDeformation : public visage::Frame {
public:
    SplineDeformation() {
        setIgnoresMouseEvents(true, false);
    }

    void draw(visage::Canvas& canvas) override {
        visage::Point center(width() / 2.0f, height() / 2.0f);
        // Adjusted scale for fewer points to create a similar size
        float scale = height() / 3.0f;

        float iTime = canvas.time();

        std::vector<visage::Point> points = deformedCircle(MAX_POINTS, iTime, center, scale);

        drawPoints(canvas, 0xff000000, points, MAX_POINTS);
        // Draw the black guide lines connecting the points
        drawLines(canvas, 0xff000000, points, MAX_POINTS);
        drawSpline(canvas, 0xffff0000, points, 2.5f); // Red spline with 2.5 width

        redraw();
    }

private:
    void drawThickLine(visage::Canvas& canvas, visage::Point p1, visage::Point p2, float thickness) const {
        visage::Point delta = p2 - p1;
        float length = sqrt(delta.x * delta.x + delta.y * delta.y);
        if (length < 1e-6f) return;

        visage::Point perp( -delta.y * (1.0f/length), delta.x * (1.0f/length) );
        float half_thickness = thickness / 2.0f;

        visage::Point v1 = p1 + perp * half_thickness;
        visage::Point v2 = p2 + perp * half_thickness;
        visage::Point v3 = p2 - perp * half_thickness;
        visage::Point v4 = p1 - perp * half_thickness;

        canvas.triangle(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y);
        canvas.triangle(v1.x, v1.y, v3.x, v3.y, v4.x, v4.y);
    }

    /**
     * CORRECTED: This function now generates a closed loop by wrapping points.
     */
    std::vector<visage::Point> deformedCircle(int count, float iTime, visage::Point center, float scale) const {
        std::vector<visage::Point> p;
        p.reserve(count + 3); // Reserve space for original points + 3 wrapped points

        // First, generate the main set of points
        for (int i = 0; i < count; ++i) {
            float a = static_cast<float>(i) / static_cast<float>(count) * TAU;
            // Using a slightly different formula to better match the target shape
            visage::Point point = visage::Point(cos(a), sin(a)) * (0.6f + 0.1f * sin(iTime * 0.5f) + 0.3f * sin(a * 3.0f + iTime));
            p.push_back(center + point * scale);
        }

        // **THE FIX**: Add the first 3 points to the end to provide neighbors for the wrap-around
        p.push_back(p[0]);
        p.push_back(p[1]);
        p.push_back(p[2]);

        return p;
    }

    visage::Point splineInterpolation(visage::Point p0, visage::Point p1, visage::Point p2, visage::Point p3, float t, float tension = 0.0f, float alpha = 0.5f) const {
        auto distance = [](visage::Point a, visage::Point b) {
            return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
        };
        
        float t01 = pow(distance(p0, p1), alpha);
        float t12 = pow(distance(p1, p2), alpha);
        float t23 = pow(distance(p2, p3), alpha);
        
        if (t01 < 1e-6f) t01 = 1.0f;
        if (t12 < 1e-6f) t12 = 1.0f;
        if (t23 < 1e-6f) t23 = 1.0f;
        
        float inv_t01 = 1.0f / t01;
        float inv_t23 = 1.0f / t23;
        float inv_t01_t12 = 1.0f / (t01 + t12);
        float inv_t12_t23 = 1.0f / (t12 + t23);

        visage::Point m1 = (p2 - p1 + (p1 - p0) * inv_t01 * t12 - (p2 - p0) * inv_t01_t12 * t12) * (1.0f - tension);
        visage::Point m2 = (p2 - p1 + (p3 - p2) * inv_t23 * t12 - (p3 - p1) * inv_t12_t23 * t12) * (1.0f - tension);

        visage::Point a = (p1 - p2) * 2.0f + m1 + m2;
        visage::Point b = (p1 - p2) * -3.0f - m1 - m1 - m2;
        visage::Point c = m1;
        visage::Point d = p1;

        return a * t * t * t + b * t * t + c * t + d;
    }

    void drawSpline(visage::Canvas& canvas, unsigned int drawColor, const std::vector<visage::Point>& p, float strokeWidth) const {
        if (p.size() < 4) return;
        canvas.setColor(drawColor);

        // CORRECTED: Loop through the original number of points to draw the full closed loop.
        for (size_t i = 0; i < MAX_POINTS; ++i) {
            visage::Point start_point = p[i+1];
            for (int j = 1; j <= 20; ++j) {
                float t = static_cast<float>(j) / 20.0f;
                // The points p[i], p[i+1], p[i+2], p[i+3] now correctly wrap around
                visage::Point end_point = splineInterpolation(p[i], p[i+1], p[i+2], p[i+3], t);
                drawThickLine(canvas, start_point, end_point, strokeWidth);
                start_point = end_point;
            }
        }
    }
    
    // Draw the black guide lines
    void drawLines(visage::Canvas& canvas, unsigned int drawColor, const std::vector<visage::Point>& p, int pointCount) const {
        canvas.setColor(drawColor);
        for(int i = 0; i < pointCount; ++i) {
            visage::Point p1 = p[i];
            // Connect last point back to the first
            visage::Point p2 = p[(i + 1) % pointCount];
            drawThickLine(canvas, p1, p2, 1.0f);
        }
    }
    
    void drawPoints(visage::Canvas& canvas, unsigned int drawColor, const std::vector<visage::Point>& p, int pointCount) const {
        canvas.setColor(drawColor);
        for(int i = 0; i < pointCount; ++i) {
            canvas.circle(p[i].x - 4.0f, p[i].y - 4.0f, 8.0f); // Larger points
        }
    }
};

/**
 * @class SplineExampleEditor
 * @brief The main application window for the spline deformation example.
 */
class SplineExampleEditor : public visage::ApplicationWindow {
public:
    SplineExampleEditor() {
        addChild(&spline_deformation_);
        spline_deformation_.layout().setMargin(0);

        onDraw() = [this](visage::Canvas& canvas) {
            canvas.setColor(0xfff0f0f0); // A light gray background
            canvas.fill(0, 0, width(), height());
        };
    }

private:
    SplineDeformation spline_deformation_;
};

/**
 * The main entry point for the application.
 */
int main(int argc, char* argv[]) {
    SplineExampleEditor editor;
    editor.setWindowDecoration(visage::Window::Decoration::Client);
    if (visage::isMobileDevice()) {
        editor.showMaximized();
    } else {
        editor.show(visage::Dimension::widthPercent(50.0f), visage::Dimension::heightPercent(60.0f));
    }

    editor.runEventLoop();

    return 0;
}