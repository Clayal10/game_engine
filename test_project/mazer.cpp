#include<fstream>
#include<iostream>
using namespace std;
int main(){
	ofstream fp;
	fp.open("maze.txt");

	for(int i=0; i<100; i++){
		for(int j=0; j<100; j++){
			fp << '#';
		}
		fp << "\n";
	}
}
