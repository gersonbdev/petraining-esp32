#include<iostream>

using namespace std;

unsigned int DispenseTimes[6][2];

int main(){
	
	int FoodRations = 3;

  	for(int x=0; x<FoodRations; x++){
		DispenseTimes[x][1] = (14/float(FoodRations+1)*(x+1)+6)*60;
		DispenseTimes[x][0] = (DispenseTimes[x][1])/60;
		DispenseTimes[x][1] = (DispenseTimes[x][1]-DispenseTimes[x][0]*60);
  	}
	
	for(int y=0; y<FoodRations; y++){
		cout<<"\nHoras "<<DispenseTimes[y][0]<<" y minutos "<<DispenseTimes[y][1]<<endl;
	}
	
	cout<<"\n"<<endl;


	return 0;
}

