#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNamedColors.h>
#include <vtkProperty.h>
#include <vtkPNGReader.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkWarpScalar.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>
#include <vtkPolyDataReader.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPointData.h>
#include <vtkCallbackCommand.h>
#include "iostream"
#include <stdio.h>
#include <stdlib.h>

#define vtkSPtr vtkSmartPointer
#define vtkSPtrNew(Var, Type) vtkSPtr<Type> Var = vtkSPtr<Type>::New();

vtkSPtrNew(renderer, vtkRenderer);
vtkSPtrNew(renderWindow, vtkRenderWindow); //Objeto para renderizar (dibujar)

using namespace std;
int visible = 0;

namespace {
    void ClickCallbackFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData) {
        vtkSPtrNew(actores, vtkActorCollection);
        vtkRenderWindowInteractor* iren =
            static_cast<vtkRenderWindowInteractor*>(caller);

        actores = renderer->GetActors();
        actores->InitTraversal();
        string key = iren->GetKeySym();
        cout << "*******************************************************" << endl;
        for (vtkIdType a = 0; a < actores->GetNumberOfItems(); ++a) {
            vtkActor* actor = actores->GetNextActor();
            cout << "Actor " << a << "/" << actores->GetNumberOfItems() << " Visible " << actor->GetVisibility() << endl;
            if (key == "Up" && visible < 10 && a == visible) {
                actor->SetVisibility(false);
                vtkActor* actor = actores->GetNextActor();
                actor->SetVisibility(true);
                a = actores->GetNumberOfItems();
                visible++;
            }
        }
        cout << "*******************************************************" << endl;
        renderer->ResetCamera();
        renderWindow->Render();
    };

}
int main(int, char* [])
{
    vtkSPtrNew(reader, vtkPolyDataReader); //Objeto para leer darchivos
    vtkSPtrNew(glyphFilter, vtkVertexGlyphFilter) //Objeto para aplicar filtros
    vtkSPtrNew(mapper, vtkPolyDataMapper); //Objeto para mapear

    char* char_arr;
    string str_obj = "../DataSources/density_bigxy_";
    string str_numero_archivo = "0000";

    for (int i = 20; i <= 30; i++)
    {
        string cargar = str_numero_archivo + to_string(i);
        int fin = cargar.length();
        cargar = str_obj + cargar.substr(static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(fin) - 4, fin) + ".vtk";

        std::cout << "Cargando imagen " << cargar << std::endl;

        char_arr = &cargar[0];
        reader->SetFileName(char_arr);

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
        actor->SetVisibility(false);

        renderer->AddActor(actor);
    }

    renderer->SetBackground(0, 0, 0);

    //Dibuja en pantalla
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("MouseEvents");

    vtkSPtrNew(renderWindowInteractor, vtkRenderWindowInteractor);
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSPtrNew(clickCallback, vtkCallbackCommand);
    clickCallback->SetCallback(ClickCallbackFunction);

    renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, clickCallback);

    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
