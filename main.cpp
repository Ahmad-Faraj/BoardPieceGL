#include <windows.h>
#include <GL/gl.h>
#include <Gl/glut.h>

#include <cmath>
#include <cstdlib>

namespace
{

    const int W = 640, H = 640;
    const float C = 80.0f;

    struct P
    {
        int r, c, t;
        bool a;
    } p[24];
    int sel = -1;

    void R(float x1, float y1, float x2, float y2, float r, float g, float b)
    {
        glColor3f(r, g, b);
        glBegin(GL_POLYGON);
        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y1, 0);
        glVertex3f(x2, y2, 0);
        glVertex3f(x1, y2, 0);
        glEnd();
    }

    void O(float cx, float cy, float ro, float r, float g, float b)
    {
        glColor3f(r, g, b);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 36; ++i)
        {
            float a = 6.2831853f * i / 36.0f;
            glVertex3f(cx + ro * std::cos(a), cy + ro * std::sin(a), 0);
        }
        glEnd();
    }

    bool dark(int r, int c) { return ((r + c) % 2) == 1; }

    int at(int r, int c)
    {
        for (int i = 0; i < 24; ++i)
            if (p[i].a && p[i].r == r && p[i].c == c)
                return i;
        return -1;
    }

    void nextSel()
    {
        if (sel < 0)
            sel = 0;
        int s = sel;
        do
        {
            sel = (sel + 1) % 24;
            if (p[sel].a)
                return;
        } while (sel != s);
        sel = -1;
    }

    void del(int i)
    {
        if (i < 0 || i >= 24 || !p[i].a)
            return;
        p[i].a = false;
        if (sel == i)
            nextSel();
    }

    void reset()
    {
        int k = 0;
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 8; ++c)
                if (dark(r, c))
                    p[k++] = {r, c, 1, true};
        for (int r = 5; r < 8; ++r)
            for (int c = 0; c < 8; ++c)
                if (dark(r, c))
                    p[k++] = {r, c, 0, true};
        sel = 0;
    }

    void moveTo(int r, int c)
    {
        if (sel < 0 || sel >= 24 || !p[sel].a)
            return;
        if (r < 0 || r > 7 || c < 0 || c > 7 || !dark(r, c) || at(r, c) != -1)
            return;
        p[sel].r = r;
        p[sel].c = c;
    }

    void draw()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        for (int r = 0; r < 8; ++r)
            for (int c = 0; c < 8; ++c)
            {
                float x1 = c * C, x2 = x1 + C, y2 = H - r * C, y1 = y2 - C;
                if (dark(r, c))
                    R(x1, y1, x2, y2, 0.30f, 0.18f, 0.10f);
                else
                    R(x1, y1, x2, y2, 0.92f, 0.86f, 0.76f);
            }

        for (int i = 0; i < 24; ++i)
        {
            if (!p[i].a)
                continue;
            float cx = p[i].c * C + C * 0.5f, cy = H - (p[i].r * C + C * 0.5f);
            if (i == sel)
                O(cx, cy, 30, 0.95f, 0.95f, 0.30f);
            if (p[i].t == 0)
                O(cx, cy, (i == sel) ? 22 : 24, 0.90f, 0.20f, 0.20f);
            else
                O(cx, cy, (i == sel) ? 22 : 24, 0.10f, 0.10f, 0.10f);
        }

        glFlush();
    }

    void mouse(int button, int state, int x, int y)
    {
        if (state != GLUT_UP)
            return;
        int r = y / (int)C, c = x / (int)C;
        if (r < 0 || r > 7 || c < 0 || c > 7)
            return;
        int h = at(r, c);
        if (button == GLUT_RIGHT_BUTTON)
            del(h);
        else if (button == GLUT_LEFT_BUTTON)
        {
            if (h != -1)
                sel = h;
            else
                moveTo(r, c);
        }
        draw();
    }

    void keyboard(unsigned char key, int, int)
    {
        if (key == 27)
            std::exit(0);
        if (key == 'r' || key == 'R')
            reset();
        else if (key == 'x' || key == 'X')
            del(sel);
        else if (key == ' ')
            nextSel();
        else if (key == 'q' || key == 'Q')
            moveTo(p[sel].r - 1, p[sel].c - 1);
        else if (key == 'e' || key == 'E')
            moveTo(p[sel].r - 1, p[sel].c + 1);
        else if (key == 'z' || key == 'Z')
            moveTo(p[sel].r + 1, p[sel].c - 1);
        else if (key == 'c' || key == 'C')
            moveTo(p[sel].r + 1, p[sel].c + 1);
        else if (key == 'w' || key == 'W')
            moveTo(p[sel].r - 1, p[sel].c);
        else if (key == 's' || key == 'S')
            moveTo(p[sel].r + 1, p[sel].c);
        else if (key == 'a' || key == 'A')
            moveTo(p[sel].r, p[sel].c - 1);
        else if (key == 'd' || key == 'D')
            moveTo(p[sel].r, p[sel].c + 1);
        draw();
    }

    void init()
    {
        glClearColor(0.08f, 0.08f, 0.08f, 0.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, (double)W, 0.0, (double)H, -1.0, 1.0);
    }

} // namespace

int main(int argc, char **argv)
{
    reset();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(180, 70);
    glutCreateWindow("BoardPiecesGL Minimal");
    init();
    glutDisplayFunc(draw);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMainLoop();
    return EXIT_SUCCESS;
}
