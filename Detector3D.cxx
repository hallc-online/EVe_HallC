///////////////////////////////////////
/*  Detector3D.h  7/17/14
    
    Ben Davis-Purcell

    Class that joins all detectors in 3D view
    and handles 3D full and partial tracks

*/
///////////////////////////////////////

#include "Detector3D.h"
#include "TMath.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "TGeoSphere.h"
#include "TWire3D.h"
#include "GetVariables.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGLViewer.h"
#include "TGLPerspectiveCamera.h"

#include <map>
#include <string>
#include <vector>

#define DEG2RAD 3.1415926/180.

using namespace std;

Detector3D::Detector3D()
{
      mgr = new TGeoManager("Geom", "Detector Stack");
      TGeoMaterial *matVacuum = new TGeoMaterial("Vacuum", 0,0,0);
      TGeoMedium *medium =  new TGeoMedium("Vacuum",1, matVacuum);
      top = mgr->MakeBox("TOP",medium,600,300,300);
      mgr->SetTopVolume(top); 
      mgr->SetVerboseLevel(0);

      TGeoBBox *top2box = new TGeoBBox("Top2box", 300, 300, 300);
      top2 = new TGeoVolume ("TOP2",top2box);

     string PN[2]={"u", "v"};  
     vector<string> PlaneNames(&PN[0],&PN[0]+2);
     
    // First MWDC
      RDC1 = new WireChamber3D((char*) "RDC1",  PlaneNames, top,mgr);

    // Second MWDC
      RDC2 = new WireChamber3D((char*) "RDC2",  PlaneNames, top,mgr);
    
   // First MWDC
      LDC1 = new WireChamber3D((char*) "LDC1",  PlaneNames, top2,mgr);

    // Second MWDC
      LDC2 = new WireChamber3D((char*) "LDC2",  PlaneNames, top2,mgr);  

      //test code: check what transformation : translation and rotation did to a box in top volume
      
      //TGeoBBox *box = new TGeoBBox("Box", 5, 5, 5); 
      TGeoBBox *box = new TGeoBBox("Box", 28.9, 39.05, 22.23); 
      TGeoTranslation boxt(0,0,0);
      TGeoRotation boxr;
      double ang = 0;
      boxr.SetAngles(90,0,90-ang,90,ang,-90);
      TGeoCombiTrans *boxCT= new TGeoCombiTrans(boxt,boxr); 
      TGeoVolume *Box = new TGeoVolume ("Box", box);
      Box ->SetLineColor(kBlack);

      top -> AddNodeOverlap(Box,1,boxCT);

      //Right Scintillation Plane  
      Rsplane = new ScintPlane3D((char*)"Rs",top);
 
      //Left Scint Plane
      Lsplane = new ScintPlane3D((char*)"Ls",top2);

      //test code: check top2 volume's origin
      TGeoTranslation top2t(-300.0, 0.0, 0.0);
      TGeoRotation top2r;
      double top2ang= 180.0;
      top2r.SetAngles(90.0, top2ang, 90.0, 90.0+ top2ang, 0.0, 0.0);
      TGeoCombiTrans *top2CT = new TGeoCombiTrans(top2t, top2r);
      top -> AddNodeOverlap(top2, 1, top2CT);
      
      // There will be 10 tracks for any fixed number of tracks in this code,
      // The reason for it is after you call CloseGeometry() function, you
      // cannot dynamically change number of TGeoVolume in root.
      for(int i=0; i<10; i++){
        TrackList.push_back(new Trajectory3D(top,mgr,i));
      }

      mgr->CloseGeometry();
      //top->Raytrace();
      top->Draw("ogl");
      TGLViewer * v = (TGLViewer *)gPad->GetViewer3D();
      if( v != NULL ) {
        static TGLViewer::ECameraType CamType=TGLViewer::kCameraPerspXOY;
        static Double_t fov = 30;
        static Double_t dolly = 300.0;            // FIXME: should be computed somehow
        static Double_t center[3] = {150, 0, 0};  // FIXME: should be fetched from bounding box
        static Double_t vRotate = -40.0*DEG2RAD;  // ROOT docs say degrees, it's Radians...
        static Double_t hRotate = -40.0*DEG2RAD;  // ROOT docs say degrees, it's Radians...
        v->SetResetCamerasOnUpdate(kFALSE);
        v->SetPerspectiveCamera (CamType,fov,dolly,center,hRotate,vRotate);
        v->SetCurrentCamera(CamType);
      }
      cout<<"Detector Created!"<<endl;
}


Detector3D::~Detector3D()
{

}


void Detector3D::ClearTracks()
{
  for (int i=0; i<TrackList.size(); i++) {
    TrackList[i]->Disable();
  }
}
