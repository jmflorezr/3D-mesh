#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkPNGReader.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkWarpScalar.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>
#include <vtkPolyDataReader.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPointData.h>
#include "iostream"

#define vtkSPtr vtkSmartPointer
#define vtkSPtrNew(Var, Type) vtkSPtr<Type> Var = vtkSPtr<Type>::New();

using namespace std;

namespace {

    // Define interaction style
    class customMouseInteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static customMouseInteractorStyle* New();
        vtkTypeMacro(customMouseInteractorStyle, vtkInteractorStyleTrackballCamera);

        virtual void OnLeftButtonDown() override
        {
            std::cout << "Presionó el botón izquierdo." << std::endl;
            // Forward events
            vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        }

        virtual void OnMiddleButtonDown() override
        {
            std::cout << "Presionó el botón del medio." << std::endl;
            // Forward events
            vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
        }

        virtual void OnRightButtonDown() override
        {
            std::cout << "Presionó el botón derecho." << std::endl;
            vtkInteractorStyleTrackballCamera::OnRightButtonDown();
        }
    };

    vtkStandardNewMacro(customMouseInteractorStyle);

} // namespace

int main(int, char* [])
{
    vtkSPtrNew(reader, vtkPolyDataReader); //Objeto para leer darchivos
    vtkSPtrNew(glyphFilter, vtkVertexGlyphFilter) //Objeto para aplicar filtros
        vtkSPtrNew(mapper, vtkPolyDataMapper); //Objeto para mapear
    vtkSPtrNew(renderWindow, vtkRenderWindow); //Objeto para renderizar (dibujar)

    char* char_arr;
    string str_obj = "../DataSources/density_bigxy_";
    string str_numero_archivo = "0000";

    for (int i = 20; i < 31; i++)
    {
        string cargar = str_numero_archivo + to_string(i);
        int fin = cargar.length();
        cargar = str_obj + cargar.substr(static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(fin) - 4, fin) + ".vtk";

        std::cout << "Cargando imagen " << cargar << std::endl;

        char_arr = &cargar[0];
        reader->SetFileName(char_arr);
    }

    reader->Update();

    glyphFilter->SetInputData(reader->GetOutput());    
    glyphFilter->Update();

    vtkSPtrNew(lut, vtkLookupTable);
    vtkIdType endID = glyphFilter->GetOutput()->GetNumberOfPoints();
    for (vtkIdType i = 0; i < 10; i++)
    {
        double red, green, blue, alpha, val;
        val = glyphFilter->GetOutput()->GetPointData()->GetArray("PointOpacity")->GetTuple1(i);
        std::cout << val << endl;
        lut->SetTableValue(i, 1, 0, 0, val);
    }    

    //Mapea archivo        
    mapper->SetInputConnection(glyphFilter->GetOutputPort());
    mapper->Update();

    //Objeto a graficar
    vtkSPtrNew(actor, vtkActor);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.0025);
    actor->GetProperty()->SetPointSize(2);

    vtkSPtrNew(renderer, vtkRenderer);
    renderer->AddActor(actor);
    renderer->SetBackground(0, 0, 0);

    //Dibuja en pantalla        
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("MouseEvents");

    vtkSPtrNew(renderWindowInteractor, vtkRenderWindowInteractor);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->ResetCamera();

    vtkNew<customMouseInteractorStyle> style;
    renderWindowInteractor->SetInteractorStyle(style);

    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
