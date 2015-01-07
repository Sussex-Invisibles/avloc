//
// Plotting tools for AV location project
// S.J.M.Peeters@sussex.ac.uk, June 2014
//
#include "include/AVLocPlot.h"
#include "include/AVLocTools.h"

#include <RAT/DS/Entry.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DB.hh>

#include <TF1.h>
#include <TFile.h>
#include <TMath.h>

#include <map>

using namespace std;

TVector2 TransformCoord( const TVector3& V1, const TVector3& V2, const TVector3& V3, const TVector2& A1, const TVector2& A2, const TVector2& A3,const TVector3& P ) {
  TVector3 xV = V2 - V1;
  TVector3 yV = ( ( V3 - V1 ) + ( V3 - V2 ) ) * 0.5;
  TVector3 zV = xV.Cross( yV ).Unit();

  double planeD = V1.Dot( zV );

  double t = planeD / P.Dot( zV );

  TVector3 localP = t*P - V1;

  TVector2 xA = A2 - A1;
  TVector2 yA = ( ( A3 - A1 ) +( A3 - A2 ) ) * 0.5;

  double convUnits = xA.Mod() / xV.Mag();

  TVector2 result;
  result = localP.Dot( xV.Unit() ) * xA.Unit() * convUnits;
  result += localP.Dot( yV.Unit() ) * yA.Unit() * convUnits + A1;
  return result;
}

TVector2 IcosProject( TVector3 pmtPos ){
  TVector3 pointOnSphere( pmtPos.X(), pmtPos.Y(), pmtPos.Z() );
  pointOnSphere = pointOnSphere.Unit();
  pointOnSphere.RotateX( -45.0 );
  // From http://www.rwgrayprojects.com/rbfnotes/polyhed/PolyhedraData/Icosahedralsahedron/Icosahedralsahedron.pdf                                                                                                                                                                                               
  const double t = ( 1.0 + sqrt( 5.0 ) ) / 2.0;
  const TVector3 V2 = TVector3( t * t, 0.0, t * t * t ).Unit();
  const TVector3 V6 = TVector3( -t * t, 0.0, t * t * t ).Unit();
  const TVector3 V12 = TVector3( 0.0, t * t * t, t * t ).Unit();
  const TVector3 V17 = TVector3( 0.0, -t * t * t, t * t ).Unit();
  const TVector3 V27 = TVector3( t * t * t, t * t, 0.0 ).Unit();
  const TVector3 V31 = TVector3( -t * t * t, t * t, 0.0 ).Unit();
  const TVector3 V33 = TVector3( -t * t * t, -t * t, 0.0 ).Unit();
  const TVector3 V37 = TVector3( t * t * t, -t * t, 0.0 ).Unit();
  const TVector3 V46 = TVector3( 0.0, t * t * t, -t * t ).Unit();
  const TVector3 V51 = TVector3( 0.0, -t * t * t, -t * t ).Unit();
  const TVector3 V54 = TVector3( t * t, 0.0, -t * t * t ).Unit();
  const TVector3 V58 = TVector3( -t * t, 0.0, -t * t * t ).Unit();
  // Faces {{ 2, 6, 17}, { 2, 12, 6}, { 2, 17, 37}, { 2, 37, 27}, { 2, 27, 12}, {37, 54, 27},                                                                                                                                                                                                                    
  // {27, 54, 46}, {27, 46, 12}, {12, 46, 31}, {12, 31, 6}, { 6, 31, 33}, { 6, 33, 17},                                                                                                                                                                                                                          
  // {17, 33, 51}, {17, 51, 37}, {37, 51, 54}, {58, 54, 51}, {58, 46, 54}, {58, 31, 46},                                                                                                                                                                                                                         
  // {58, 33, 31}, {58, 51, 33}}                                                                                                                                                                                                                                                                                 
  vector<TVector3> IcosahedralCentres;
  IcosahedralCentres.push_back( ( V2 + V6 + V17 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V2 + V12 + V6 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V2 + V17 + V37 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V2 + V37 + V27 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V2 + V27 + V12 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V37 + V54 + V27 ) * ( 1.0 / 3.0 ) );

  IcosahedralCentres.push_back( ( V27 + V54 + V46 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V27 + V46 + V12 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V12 + V46 + V31 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V12 + V31 + V6 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V6 + V31 + V33 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V6 + V33 + V17 ) * ( 1.0 / 3.0 ) );

  IcosahedralCentres.push_back( ( V17 + V33 + V51 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V17 + V51 + V37 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V37 + V51 + V54 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V58 + V54 + V51 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V58 + V46 + V54 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V58 + V31 + V46 ) * ( 1.0 / 3.0 ) );

  IcosahedralCentres.push_back( ( V58 + V33 + V31 ) * ( 1.0 / 3.0 ) );
  IcosahedralCentres.push_back( ( V58 + V51 + V33 ) * ( 1.0 / 3.0 ) );

  vector<double> distFromCentre;
  unsigned int uLoop;
  for( uLoop = 0; uLoop < IcosahedralCentres.size(); uLoop++ ){
    distFromCentre.push_back( ( IcosahedralCentres[uLoop] - pointOnSphere ).Mag() );
  }
  const int face = min_element( distFromCentre.begin(), distFromCentre.end() ) - distFromCentre.begin() + 1;
  TVector2 resultPosition;
  switch(face)
    {
    case 1://{ 2, 6, 17}
      resultPosition = TransformCoord( V2, V6, V17, A2a, A6, A17a, pointOnSphere );
      break;
    case 2://{ 2, 12, 6}                        
      resultPosition = TransformCoord( V2, V12, V6, A2a, A12a, A6, pointOnSphere );
      break;
    case 3://{ 2, 17, 37}
      resultPosition = TransformCoord( V2, V17, V37, A2b, A17b, A37, pointOnSphere );
      break;
    case 4://{ 2, 37, 27}
      resultPosition = TransformCoord( V2, V37, V27, A2b, A37, A27, pointOnSphere );
      break;
    case 5://{ 2, 27, 12}
      resultPosition = TransformCoord( V2, V27, V12, A2b, A27, A12e, pointOnSphere );
      break;
    case 6://{37, 54, 27}
      resultPosition = TransformCoord( V37, V54, V27, A37, A54, A27, pointOnSphere );
      break;
    case 7://{27, 54, 46}
      resultPosition = TransformCoord( V27, V54, V46, A27, A54, A46, pointOnSphere );
      break;
    case 8://{27, 46, 12}
      resultPosition = TransformCoord( V27, V46, V12, A27, A46, A12d, pointOnSphere );
      break;
    case 9://{12, 46, 31}
      resultPosition = TransformCoord( V12, V46, V31, A12c, A46, A31, pointOnSphere );
      break;
    case 10://{12, 31, 6}
      resultPosition = TransformCoord( V12, V31, V6, A12b, A31, A6, pointOnSphere );
      break;
    case 11://{ 6, 31, 33}
      resultPosition = TransformCoord( V6, V31, V33, A6, A31, A33, pointOnSphere );
      break;
    case 12://{ 6, 33, 17}
      resultPosition = TransformCoord( V6, V33, V17, A6, A33, A17a, pointOnSphere );
      break;
    case 13://{17, 33, 51}
      resultPosition = TransformCoord( V17, V33, V51, A17a, A33, A51a, pointOnSphere );
      break;
    case 14://{17, 51, 37}
      resultPosition = TransformCoord( V17, V51, V37, A17b, A51e, A37, pointOnSphere );
      break;
    case 15://{37, 51, 54}
      resultPosition = TransformCoord( V37, V51, V54, A37, A51d, A54, pointOnSphere );
      break;
    case 16://{58, 54, 51}
      resultPosition = TransformCoord( V58, V54, V51, A58, A54, A51c, pointOnSphere );
      break;
    case 17://{58, 46, 54}
      resultPosition = TransformCoord( V58, V46, V54, A58, A46, A54, pointOnSphere );
      break;
    case 18://{58, 31, 46}
      resultPosition = TransformCoord( V58, V31, V46, A58, A31, A46, pointOnSphere );
      break;
    case 19://{58, 33, 31}
      resultPosition = TransformCoord( V58, V33, V31, A58, A33, A31, pointOnSphere );
      break;
    case 20://{58, 51, 33}
      resultPosition = TransformCoord( V58, V51, V33, A58, A51b, A33, pointOnSphere );
      break;
    }
  return TVector2( resultPosition.X(), 2.0 * resultPosition.Y() );
}


// use ntuple to plot flat map
TH2D * flatmap_ntuple(TNtuple * ntuple, double distance, int fibre_nr, int sub_nr, double time_min, double time_max, bool in)
{
  // PMT info
  RAT::Log::Init("/dev/null");
  RAT::DB* db = RAT::DB::Get();
  assert(db);
  char* ratroot = getenv("RATROOT");
  if (ratroot == static_cast<char*>(NULL)) {
    cerr << "Environment variable $RATROOT must be set" << endl;
    assert(ratroot);
  }
  string rat     = string(ratroot);
  string pmtfile = rat;
  pmtfile += "/data/pmt/snoman.ratdb";
  db->LoadFile(pmtfile);
  RAT::DBLinkPtr pmtInfo = db->GetLink("PMTINFO");
  assert(pmtInfo);
  vector<double> xPos = pmtInfo->GetDArray("x");
  vector<double> yPos = pmtInfo->GetDArray("y");
  vector<double> zPos = pmtInfo->GetDArray("z");

  // Loop over ntuple
  Double_t pmt_hits[10000] = {0};

  unsigned int nev = ntuple->GetEntries();
  for (unsigned int i=0 ; i < nev ; ++i) {
    ntuple->GetEntry(i);
    int  fibre  = (int)ntuple->GetArgs()[0];
    int    sub  = (int)ntuple->GetArgs()[1];
    int    lcn  = (int)ntuple->GetArgs()[2];
    double time = (double)ntuple->GetArgs()[3];
    double dis  = (double)ntuple->GetArgs()[4]; 
    bool in_distance;
    if ( in ) {
      dis < distance ? in_distance = true : in_distance = false;
    }
    else {
      dis > distance ? in_distance = true : in_distance = false;

    }
    if (in_distance && fibre == fibre_nr && sub == sub_nr && time >= time_min && time < time_max) {
      pmt_hits[lcn] += 1;
    }
  }

  // Make plot
  const int xbins = 300;
  const int ybins = 300;
  TH2D *hflatmap = new TH2D("hflatmap","SNO+ flatmap",xbins, 0 , 1, ybins, 0 , 1);
  for (unsigned int i = 0; i < 10000; i++){
    TVector3 pmtPos(xPos[i],yPos[i],zPos[i]);
    TVector2 icosProj = IcosProject(pmtPos);
    int xbin = int((1-icosProj.X())*xbins);
    int ybin = int((1-icosProj.Y())*ybins);
    int bin = hflatmap->GetBin(xbin,ybin);
    hflatmap->SetBinContent(bin,pmt_hits[i]);
  }
  return hflatmap;
}


// use ntuple to plot the time histograms
void time_histograms(TNtuple * ntuple, double distance, int fibre_nr, int sub_nr)
{
  // Loop over ntuple
  TH1I * histo_map[10000];
  for (unsigned int i = 0 ; i < 10000 ; ++i ) histo_map[i] = NULL;
  unsigned int nev = ntuple->GetEntries();
  for (unsigned int i = 0 ; i < nev ; ++i) {
    ntuple->GetEntry(i);
    double dist = (double)ntuple->GetArgs()[4]; 
    if ( dist < distance ) {
      int    lcn  = (int)ntuple->GetArgs()[2];
      double time = (double)ntuple->GetArgs()[3]; 
      int  fibre  = (int)ntuple->GetArgs()[0];
      int  sub    = (int)ntuple->GetArgs()[1];
      if ( histo_map[lcn] == NULL ) {
        char name[128];
        sprintf(name,"pmt%i",lcn);
        histo_map[lcn] = new TH1I(name,name,51,-0.5,50.5);
        histo_map[lcn]->SetXTitle("time (ns)");
      }
      if ( fibre == fibre_nr && sub == sub_nr && time-250. > 0. && time-250. < 50. ) {
	histo_map[lcn]->Fill(time-250.);
      }
    }
  }

  // save histograms to file (needs to open!)
  TH1D * time_summary = new TH1D("time_summary","average hit time for each PMT",
				 10001,-0.5,10000.5);
  TH1D * time_histo = new TH1D("time_histo","time distribution for reflections",
			       501,-0.5,50.5);
  time_summary->SetXTitle("LCN");
  time_summary->SetYTitle("hit_time (ns)");
  for (unsigned int i = 0 ; i < 10000 ; ++i ) {
    if (histo_map[i] != NULL ) {
      // if at least 30 entries, calculate mean and rms
      if (histo_map[i]->GetEntries() > 30 ) {
	histo_map[i]->Fit("gaus");
	histo_map[i]->Write();
	TF1 * f = histo_map[i]->GetFunction("gaus");
	assert(f);
	double mu = f->GetParameter(1);
	double si = f->GetParameter(2);
	time_summary->SetBinContent(i+1,mu);
	time_summary->SetBinError  (i+1,si);
	time_histo->Fill(mu,1./(si*si));
      }	
    }
  }
  time_summary->Write();
  time_histo->Fit("gaus");
  time_histo->Write();
}

void plot_offset(TNtuple * ntuple, double distance, int fibre_nr, int sub_nr)
{
  LEDInfo   led    = GetLEDInfoFromFibreNr(fibre_nr, sub_nr);
  PMTInfo pmt_info = GetPMTpositions();

  // effective refractive index:
  // need to get this from the database but is in data now ... hardcoded, i.e. improve!!
  PhysicsNr n_h2o; 
  n_h2o.value = 1.3637; 
  n_h2o.error = 0.0021;
  // Loop over ntuple
  TH1I * histo_map[10000];
  for (unsigned int i = 0 ; i < 10000 ; ++i ) histo_map[i] = NULL;
  unsigned int nev = ntuple->GetEntries();
  for (unsigned int i = 0 ; i < nev ; ++i) {
    ntuple->GetEntry(i);
    double dist = (double)ntuple->GetArgs()[4]; 
    if ( dist < distance ) {
      int    lcn    = (int)ntuple->GetArgs()[2];
      double time   = (double)ntuple->GetArgs()[3]; 
      int    fibre  = (int)ntuple->GetArgs()[0];
      if ( histo_map[lcn] == NULL ) {
        char name[128];
        sprintf(name,"pmt%i",lcn);
        histo_map[lcn] = new TH1I(name,name,51,-25.5,25.5);
        histo_map[lcn]->SetXTitle("time (ns)");
      }
      if ( fibre == fibre_nr && time-250. > 0. && time-250. < 50. ) {
	TVector3 PMT_pos(pmt_info.x_pos[lcn],pmt_info.y_pos[lcn],pmt_info.z_pos[lcn]);
	PhysicsNr tof = TimeOfFlight(led.position, PMT_pos, n_h2o, 1.);
	histo_map[lcn]->Fill(time-250.-tof.value);
      }
    }
  }

  // save histograms to file (needs to open!)
  TH1D * time_summary = new TH1D("time_summary","average hit time for each PMT",
				 10001,-0.5,10000.5);
  char title[128];
  sprintf(title,"time distribution for reflections, fibre %i-%i",fibre_nr,sub_nr);
  TH1D * time_histo = new TH1D("time_histo",title, 101,-10.05,10.05);
  time_summary->SetXTitle("LCN");
  time_summary->SetYTitle("hit_time (ns)");
  for (unsigned int i = 0 ; i < 10000 ; ++i ) {
    if (histo_map[i] != NULL ) {
      // if at least 30 entries, calculate mean and rms
      if (histo_map[i]->GetEntries() > 30 ) {
	histo_map[i]->Fit("gaus");
	histo_map[i]->Write();
	TF1 * f = histo_map[i]->GetFunction("gaus");
	assert(f);
	double mu = f->GetParameter(1);
	double si = f->GetParameter(2);
	time_summary->SetBinContent(i+1,mu);
	time_summary->SetBinError  (i+1,si);
	time_histo->Fill(mu,1./(si*si));
      }	
    }
  }
  time_summary->Write();
  time_histo->Fit("gaus");
  time_histo->SetXTitle("ns");
  time_histo->Write();

}