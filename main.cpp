#include <iostream>


#include "SkyMapMatching.h"

using namespace std;

int main() {
//    int range1 = 10;
//    float range2 = 10.0;
//    float range3 = 3.2;
//    float x1 = 34.78;
//    cout<<fmod(x1,range1)<<endl;
//    cout<<fmod(x1,range2)<<endl;
//    cout<<fmod(x1,range3)<<endl;

    SkyMapMatching SMM;

    string filename = "D:\\Program Files\\CLion\\SkyMap\\Data\\skymaps.csv";
    string picture = "D:\\Program Files\\CLion\\SkyMap\\Data\\observation_00.csv";
    SMM.LoadSky(filename);
    SMM.LoadImage(picture);
    StarPoint c(0,140.0,0.0,0.0);
    float len = 20.0,width = 20.0;
    SMM.GenerateSimImage(c,len,width);
    SMM.SelectTargetStar();
    SMM.Match();
    if( SMM.Check() ) cout<<"Good!"<<endl;
    else cout<<"Sad!"<<endl;

    return 0;
}