#include "GUIMyFrame1.h"
#include <vector>
#include <fstream>
#include "vecmat.h"
#include <cmath>

struct Point {
    float x, y, z;
    Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Color {
    int R, G, B;
    Color(int _R, int _G, int _B) : R(_R), G(_G), B(_B) {}
};

struct Segment {
    Point begin, end;
    Color color;
    Segment(Point _begin, Point _end, Color _color) : begin(_begin), end(_end), color(_color) {}
};

std::vector<Segment> data;

GUIMyFrame1::GUIMyFrame1( wxWindow* parent ) : MyFrame1( parent )
{
    this->SetMinSize(wxSize(800, 600));
    this->SetSize(wxSize(800, 600));
    WxSB_TranslationX->SetRange(0, 200); WxSB_TranslationX->SetValue(100);
    WxSB_TranslationY->SetRange(0, 200); WxSB_TranslationY->SetValue(100);
    WxSB_TranslationZ->SetRange(0, 200); WxSB_TranslationZ->SetValue(100);

    WxSB_RotateX->SetRange(0, 360); WxSB_RotateX->SetValue(0);
    WxSB_RotateY->SetRange(0, 360); WxSB_RotateY->SetValue(0);
    WxSB_RotateZ->SetRange(0, 360); WxSB_RotateZ->SetValue(0);

    WxSB_ScaleX->SetRange(1, 200); WxSB_ScaleX->SetValue(100);
    WxSB_ScaleY->SetRange(1, 200); WxSB_ScaleY->SetValue(100);
    WxSB_ScaleZ->SetRange(1, 200); WxSB_ScaleZ->SetValue(100);
}

void GUIMyFrame1::WxPanel_Repaint( wxUpdateUIEvent& event )
{
    Repaint();
}

void GUIMyFrame1::m_button_load_geometry_click( wxCommandEvent& event )
{
    wxFileDialog WxOpenFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("Geometry file (*.geo)|*.geo"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (WxOpenFileDialog.ShowModal() == wxID_OK)
    {
        double x1, y1, z1, x2, y2, z2;
        int r, g, b;

        std::ifstream in(WxOpenFileDialog.GetPath().ToAscii());
        if (in.is_open())
        {
            data.clear();
            while (!in.eof())
            {
                in >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> r >> g >> b;
                data.push_back(Segment(Point(x1, y1, z1), Point(x2, y2, z2), Color(r, g, b)));
            }
            in.close();
        }
    }
}

void GUIMyFrame1::Scrolls_Updated( wxScrollEvent& event )
{
    WxST_TranslationX->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationX->GetValue() - 100) / 50.0));
    WxST_TranslationY->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationY->GetValue() - 100) / 50.0));
    WxST_TranslationZ->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationZ->GetValue() - 100) / 50.0));

    WxST_RotateX->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateX->GetValue()));
    WxST_RotateY->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateY->GetValue()));
    WxST_RotateZ->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateZ->GetValue()));

    WxST_ScaleX->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleX->GetValue() / 100.0));
    WxST_ScaleY->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleY->GetValue() / 100.0));
    WxST_ScaleZ->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleZ->GetValue() / 100.0));


    Repaint();
}

void GUIMyFrame1::Repaint()
{
    wxClientDC dc(WxPanel);
    wxBufferedDC dc(&dc);

    dc.SetBackground(wxBrush(wxColor(255, 255, 255)));
    dc.Clear();

    int w, h;
    WxPanel->GetSize(&w, &h);
    dc.SetDeviceOrigin(w / 2, h / 2); 

    // 2. Pobranie wartości z suwaków
    // Translacja (zakładamy zakres np. -2.0 do 2.0 dla widoczności)
    double tx = (WxSB_TranslationX->GetValue() - 100) / 50.0;
    double ty = (WxSB_TranslationY->GetValue() - 100) / 50.0;
    // Translacja Z: dodajemy offset, żeby obiekt nie był "w oku" kamery na starcie
    double tz = (WxSB_TranslationZ->GetValue() - 100) / 50.0 + 2.0; 

    // Obroty (w radianach)
    double rx = WxSB_RotateX->GetValue() * M_PI / 180.0;
    double ry = WxSB_RotateY->GetValue() * M_PI / 180.0;
    double rz = WxSB_RotateZ->GetValue() * M_PI / 180.0;

    // Skalowanie
    double sx = WxSB_ScaleX->GetValue() / 100.0;
    double sy = WxSB_ScaleY->GetValue() / 100.0;
    double sz = WxSB_ScaleZ->GetValue() / 100.0;

    // 3. Budowa macierzy transformacji
    // Macierz Skalowania
    Matrix4 matScale;
    matScale.data[0][0] = sx;
    matScale.data[1][1] = sy;
    matScale.data[2][2] = sz;

    // Macierz Obrotu X
    Matrix4 matRotX;
    matRotX.data[1][1] = cos(rx); 
    matRotX.data[1][2] = -sin(rx);
    matRotX.data[2][1] = sin(rx); 
    matRotX.data[2][2] = cos(rx);

    // Macierz Obrotu Y
    Matrix4 matRotY;
    matRotY.data[0][0] = cos(ry); 
    matRotY.data[0][2] = sin(ry);
    matRotY.data[2][0] = -sin(ry); 
    matRotY.data[2][2] = cos(ry);

    // Macierz Obrotu Z
    Matrix4 matRotZ;
    matRotZ.data[0][0] = cos(rz); 
    matRotZ.data[0][1] = -sin(rz);
    matRotZ.data[1][0] = sin(rz); 
    matRotZ.data[1][1] = cos(rz);

    // Macierz Translacji
    Matrix4 matTrans;
    matTrans.data[0][3] = tx;
    matTrans.data[1][3] = ty;
    matTrans.data[2][3] = tz;

    // Złożenie transformacji: M = T * Rx * Ry * Rz * S
    // Uwaga: Klasa Matrix4 z vecmat.h ma proste mnożenie. 
    // Sprawdzamy czy mnożymy M1 * M2.
    Matrix4 transform = matTrans * matRotX * matRotY * matRotZ * matScale;

    // 4. Rysowanie wektorów
    for (const auto& segment : data)
    {
        // Tworzenie wektorów początkowych (jedynka na końcu dla współrzędnych jednorodnych)
        Vector4 v1, v2;
        v1.Set(segment.begin.x, segment.begin.y, segment.begin.z);
        v2.Set(segment.end.x, segment.end.y, segment.end.z);

        // Aplikacja transformacji
        v1 = transform * v1;
        v2 = transform * v2;

        // 5. Przycinanie (Clipping) względem płaszczyzny rzutowania (rzutnia)
        // Zakładamy Near Plane na z = 0.1 (aby uniknąć dzielenia przez 0 i obiektów za kamerą)
        double zNear = 0.1;

        // Jeśli oba punkty są za kamerą, pomijamy odcinek
        if (v1.GetZ() < zNear && v2.GetZ() < zNear)
            continue;

        // Jeśli jeden punkt jest za kamerą, przycinamy go do płaszczyzny
        if (v1.GetZ() < zNear)
        {
            double t = (zNear - v1.GetZ()) / (v2.GetZ() - v1.GetZ());
            v1.data[0] = v1.GetX() + (v2.GetX() - v1.GetX()) * t;
            v1.data[1] = v1.GetY() + (v2.GetY() - v1.GetY()) * t;
            v1.data[2] = zNear;
        }
        else if (v2.GetZ() < zNear)
        {
            double t = (zNear - v2.GetZ()) / (v1.GetZ() - v2.GetZ());
            v2.data[0] = v2.GetX() + (v1.GetX() - v2.GetX()) * t;
            v2.data[1] = v2.GetY() + (v1.GetY() - v2.GetY()) * t;
            v2.data[2] = zNear;
        }

        // 6. Rzutowanie perspektywiczne i mapowanie na ekran
        // x' = x / z, y' = y / z
        // Dodatkowo skalujemy wynik (np. * 300), aby dopasować go do pikseli ekranu
        // Odwracamy oś Y ( * -1), bo na ekranie Y rośnie w dół.
        
        double focus = 300.0; // Ogniskowa / skala rzutowania

        double x1_2d = (v1.GetX() / v1.GetZ()) * focus;
        double y1_2d = (v1.GetY() / v1.GetZ()) * focus * -1; // Odwrócenie Y

        double x2_2d = (v2.GetX() / v2.GetZ()) * focus;
        double y2_2d = (v2.GetY() / v2.GetZ()) * focus * -1; // Odwrócenie Y

        // Rysowanie linii
        dc.SetPen(wxPen(wxColor(segment.color.R, segment.color.G, segment.color.B)));
        dc.DrawLine(x1_2d, y1_2d, x2_2d, y2_2d);
    }
}