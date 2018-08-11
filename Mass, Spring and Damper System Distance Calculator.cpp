//Kenneth Perera
//Mass, Spring and Damper System Distance Calculator

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include "c:\dislin\dislin.h" // DOWNLOAD DISLIN HERE: http://www.mps.mpg.de/dislin/distributions
using namespace std;

//global constants
const int dataRow = 3;
const int dataCol = 5;
const double PI = atan(1) * 4;

//systemForce: Class that holds the frequency, time and initial force
class systemForce 
{
	public:
		systemForce () //CONSTRUCTOR
		{
			frequency = 0.0;
			force = 0.0;
			time = 0.0;
			force_initial = 0.0;
		}

		//Getters and Setters
		void setFrequency (double input);		
		double getFrequency();
		void setTime (double input);
		double getTime ();
		void setforce_initial (double input);
		double getforce_initial ();
		
		//Function that gets the final force of the system.
		double forceOfTheSystem ();
	
	//Private Variables
	private:
		double frequency;
		double force;
		double time;
		double force_initial;
};

//systemSuspension: Class that holds the stiffness, damping value, mass and constants
class systemSuspension 
{
	public:
		systemSuspension () //CONSTRUCTOR
		{
			stiffness = 0.0;
			damping = 0.0;
			mass = 0.0;
			constantA = 0.0;
			constantB = 0.0;
			constantC = 0.0;
		}
		
		//Getters and setters
		void setStiffness (double input);
		double getStiffness();
		void setDamping (double input);
		double getDamping ();
		void setMass (double input);
		double getMass ();
		
		//Functions that get constants that do not change.
		double getConstantA (double deltaTime);
		double getConstantB (double deltaTime);
		double getConstantC (double deltaTime);
	
	//Private Variables
	private:
		double stiffness;
		double damping;
		double mass;
		double constantA;
		double constantB;
		double constantC;
};

//wholeSystem: inherits the systemForce and systemSuspension classes
class wholeSystem : public systemForce, public systemSuspension
{
};

//Function Prototypes
void introduction(double deltaTime);
void inputFileData(string& inputFile, ifstream& READ, double** data);
void setAndDeltaTime (int& set, double& deltatTime, char choice);
void calculatePosition (double** data, int set, double deltaTime, wholeSystem System, char& choice, int incre);
void outputFileData(string& outputFile, ofstream& PRINT, float* position, float* timeArray, int set, const int MAX_SIZE, char& choice, int incre, int duration);
void grapher(float position[], float timeArray[], int set, int duration, const int MAX_SIZE);

int main ()
{		
	//Object
	wholeSystem System;
	
	//Variables for functions
	string inputFile = "";
	ifstream READ;
	double deltaTime = 0.0001;
	int set = 0;
	int incre = 2;
	char choice = 'Y';

	//Data Array
	double** data = new double* [dataRow]; // 2-D dynamically allocated array!
	for (int index = 0; index < dataRow; index++)
	{
		data[index] = new double [dataCol];
	}
	
	//FUNCTION CALLS
	introduction(deltaTime);
	inputFileData(inputFile, READ, data);	
	
	while (choice == 'Y')
	{
		setAndDeltaTime (set, deltaTime, choice);
		calculatePosition (data, set, deltaTime, System, choice, incre);
		incre++;
	}		
	
	cin >> inputFile;
	return 0;
}

//This function explains and introduces the program.
void introduction(double deltaTime)
{
	//Introducing the program
	cout << "This is a program that calculates the oscilating distance of a "
		 << "mass, spring and damper system. You will be prompted to enter the "
		 << "following data: \n" << endl;
		 
	cout << "1. The input file's name, where the following data is read:\n" << endl;
	cout << "-C (kg/s), the damping value" << endl;
	cout << "-k (N/m), the stiffness" << endl;
	cout << "-Fo (N), the initial force" << endl;
	cout << "-f (Hz), the frequency" << endl;
	cout << "-Mass (kg), the mass of the object\n" << endl;
	
	cout << "These values will be used to calculate:\n" << endl;
	cout << "-F (N), the final force" << endl;
	cout << "-position (m), at specific points of time" << endl;
	cout << "-Various constants used to calculate the position of the obejct\n" << endl;
	
	cout << "NOTE: This program will check for the input file's existence. If the input file       is not found the program will keep "
		 << "checking for the input file's existence      until a file is finally found.\n" << endl;
	
	cout << "2. The set you wish to use for a specific run" << endl;
	cout << "-You may choose from sets 1, 2 or 3." << endl;
	cout << "-This choice may NOT be less than 1 or greater than 3.\n" << endl;
	
	cout << "3. You will have a choice to enter the value of dt" << endl;
	cout << "-The value of dt used if you choose not to enter your own: " << deltaTime << "s" << endl;
	cout << "-This value may NOT be negative" << endl; 
	cout << "-If you enter a value for dt it must be a small value that is less than or equal to 0.0001s\n" << endl;
	
	cout << "4. The number of seconds the system will oscilate" << endl;
	cout << "-This value may NOT be negative\n" << endl;
	
	cout << "5. The output file's name, which will print: " << endl;
	cout << "-The position of the mass at each point in time (m)\n" << endl;
	
	cout << "NOTE: The program will NOT check for the output file's existence. If the output       file is not found the program "
		 << "will create a brand new text file in\n      accordance to the name of output file entered.\n" << endl;
		 
	cout << "6. How often values will be printed into the output file. (i.e: every 50th\n   value)" << endl;	
	cout << "-This value may NOT be negative" << endl;
	cout << "-This value can only be a factor of 50 and less than or equal to 1000\n" << endl;

	cout << "7. You will have a choice to re-run the program if necessary" << endl;
	cout << "-You may rerun the program as many times as you wish.\n" << endl;
	
	cout << "The input file should contain three sets of data. Which should contain the\nfollowing:\n" << endl;
	cout << "C(kg/s)" << setw(10) << "k(N/m)" << setw(11) << "Fo(N)" << setw(8) << "f(Hz)" << setw(13) << "Mass (kg)" << setw(5) << "Set" << endl;
	cout << "------------------------------------------------------" << endl;
	cout << "2" << setw(12) << "25" << setw(11) << "1" << setw(10) << "2" << setw(9) << "0.5" << setw(9) << "1" << endl;
	cout << "4" << setw(12) << "35" << setw(11) << "1" << setw(10) << "20" << setw(9) << "1.5" << setw(9) << "2" << endl;
	cout << "8" << setw(12) << "45" << setw(11) << "1" << setw(10) << "200" << setw(9) << "2.5" << setw(9) << "3" << endl;
	cout << endl;
	
	cout << "The program will update you on which simulation you are currently on with each\niteration and with each simulation preformed.\n" << endl;
	
	cout << "The program will provide a Position vs. Time plot once the values have been\nprinted in a PDF file.\n"<< endl;
}

//FUNCTION DEFINITIONS.

//This function processes input file data for the program.
void inputFileData(string& inputFile, ifstream& READ, double** data)
{	

	cout << "SIMULATION #1" << endl;
	cout << "-------------------------\n" << endl;	
	
	//Getting inputfile name
	cout << "Please enter the file for reading data: ";
	cin >> inputFile;

	cout << endl;
	
	cout << "Now attempting to open: " << inputFile << endl;
	cout << endl;
	
	READ.open(inputFile.c_str());
	
	//Checking for inputFile's existence.
	while (READ.fail())
	{
		cout << "The file wasn't found! Please try again: ";
		cin >> inputFile;
		
		READ.open(inputFile.c_str());
		
		if (!READ.fail())
		{
			cout << "\nThe input file was found!\n" << endl;
		}
	}
	
	//Processing data into the data array.
	cout << "Now reading: " << inputFile << endl;
	cout << endl;
	
	for (int i = 0; i < dataRow; i++)
	{
		for (int j = 0; j < dataCol; j++)
		{
			READ >> data[i][j];
		}
	}
	
	cout << "Now done reading the data from " << inputFile << endl;
	cout << endl;
	
	cout << "Now closing: " << inputFile << endl;
	cout << endl;
	
	READ.close();
	
}
	
//This function sets the data set and delta time
void setAndDeltaTime (int& set, double& deltaTime, char choice)
{
	//Getting data set.
	cout << "Please enter the set of data that will be used: ";
	cin >> set;
	
	while (set < 1 || set > 3)
	{
		cout << "This value must be 1, 2 or 3. Please try again: ";
		cin >> set;
	}
	
	cout << endl;
	
	set = set - 1;
	
	//Getting data time if the user chooses
	cout << "Would you like to enter your own delta time? (Y/N): ";
	cin >> choice;
	
	while (choice != 'Y' && choice != 'N')
	{
		cout << "This is not a valid choice. Please try again (Y/N): ";
		cin >> choice;
	}
	
	cout << endl;
	
	if (choice == 'Y')
	{
		cout << "You have chosen yes. Please enter the delta time value you wish to use: ";
		cin >> deltaTime;
		
		while (deltaTime <= 0 || deltaTime >= 0.0001)
		{
			if (deltaTime <= 0)
			{
				cout << "The value must be greater than 0. Please try again: ";
				cin >> deltaTime;
			}
			
			else
			{
				cout << "The value must be less than 0.0001. Please try again: ";
				cin >> deltaTime;
			}
		}
		
		cout << endl;
	}
	
	else
	{
		cout << "You have chosen no; therefore, " << deltaTime << "s will be used in this run." << endl;
		cout << endl; 
	}
}

//This function calculates the position of an oscillating system.
void calculatePosition (double** data, int set, double deltaTime, wholeSystem System, char& choice, int incre)
{
	//VARIABLES FOR outputFileData FUNCTION.
	string outputFile = "";
	ofstream PRINT;
	
	//Setting values in the objects
	System.setFrequency (data[set][3]);
	System.setforce_initial (data[set][2]);
	System.setStiffness(data[set][1]);
	System.setDamping(data[set][0]);
	System.setMass(data[set][4]);
	
	//Variables
	double duration = 0;
	double time = 0.0;
	
	//Getting simulation duration time.
	cout << "Please enter the amount of time the simulation will take place: ";
	cin >> duration;
	
	while (duration <= 0)
	{
		cout << "The value must be greater than 0. Please try again: ";
		cin >> duration;
	}
	
	cout << endl;
	
	//Getting size of the position array
	const int MAX_SIZE = duration / deltaTime;
	
	//Dunamically Allocated Arrays
	float* position = new float [MAX_SIZE];
	float* timeArray = new float [MAX_SIZE];
	
	//Setting position[0] = 0.0
	position[0] = 0.0;
	timeArray[0] = 0.0;
	System.setTime(deltaTime);
	
	//Setting position[1]
	time = time + deltaTime;	
	System.setTime(time);
	position[1] = System.getConstantA (deltaTime) * (System.forceOfTheSystem());
	timeArray[1] = time;

	//Calculating position at different times
	for (int index = 1; index <= MAX_SIZE; index++)
	{	
		time = time + deltaTime;	
		System.setTime(time);
		position[index + 1] = System.getConstantA (deltaTime) * (System.forceOfTheSystem() - (System.getConstantB (deltaTime) * position[index - 1]) - (System.getConstantC (deltaTime) * position[index]));
		timeArray[index] = time;
	} 
	
	//Printing data into the outputfile.
	outputFileData(outputFile, PRINT, position, timeArray, set, MAX_SIZE, choice, incre, duration);
}

//This function outputs data to a file.
void outputFileData(string& outputFile, ofstream& PRINT, float* position, float* timeArray, int set, const int MAX_SIZE, char& choice, int incre, int duration)
{
	
	//Variables
	int printingValue;

	//Getting outputFile name
	cout << "Please enter the file for printing data: ";
	cin >> outputFile;
	cout << endl;
	
	cout << "Please enter how often the data values will print: ";
	cin >> printingValue;
	
	while (printingValue % 50 != 0 || (printingValue <= 0 || printingValue > 1000))
	{
		if (printingValue % 50 != 0)
		{
			cout << "The value must be a factor of 50. Please try again: ";
			cin >> printingValue;
		}
		
		else if (printingValue <= 0)
		{
			cout << "The value must be greater than 0. Please try again: ";
			cin >> printingValue;
		}
		
		else
		{
			cout << "The value must be less than or equal to 1000. Please try again: ";
			cin >> printingValue;	
		}
	}
	
	cout << endl;

	cout << "Now opening: " << outputFile << endl;
	cout << endl;
	
	PRINT.open(outputFile.c_str());

	cout << "Now printing: " << outputFile << endl;
	cout << endl;
	
	//Printing data to the outputFile
	PRINT << "Set used: " << set + 1 << endl;
	PRINT << endl;
	
	PRINT << "Position (m)" << setw(36) << "Time (s)" << endl;
	PRINT << "--------------------------------------------------------" << endl;
	PRINT << "0" << setw(46) << "0" << endl;
	
	for (int index = 1; index <= MAX_SIZE; index++)
	{
		if (index % printingValue == 0)
		{
			PRINT  << left << setw(20) << fixed << showpoint << setprecision(10) << position[index];
			PRINT  << setw(25) << right <<fixed << showpoint << setprecision(4) << timeArray[index] << endl;
		}
	}
	
	PRINT << endl;
	
	cout << "Now done printing the data to " << outputFile << endl;
	cout << endl;
	
	cout << "Now closing: " << outputFile << endl;
	cout << endl;
	
	PRINT.close();
	
	cout << "Would you like to print out a Position vs. Time plot? (Y/N): ";
	cin >> choice;
	
	while (choice != 'Y' && choice != 'N')
	{
		cout << "This is not a valid choice. Please try again (Y/N): ";
		cin >> choice;
	}
	
	if (choice == 'Y')
	{
		grapher (position, timeArray, set, duration, MAX_SIZE);
	}
	
	else
	{
		cout << endl;
	}
	
	//Prompting for re-run
	cout << "Would you like to re-run this program? (Y/N): ";
	cin >> choice;
	
	while (choice != 'Y' && choice != 'N')
	{
		cout << "This is not a valid choice. Please try again (Y/N): ";
		cin >> choice;
	}
	
	cout << endl;
	
	if (choice == 'N')
	{	
		cout << "Times simulated: " << incre - 1 << endl;
		cout << endl;
		
		cout << "Now ending the program. . ." << endl;
	}
	
	else 
	{		
		cout << "SIMULATION #" << incre << endl;
		cout << "-------------------------" << endl;	
	}
	
	cout << endl;
}

//Function to create a graph.
void grapher (float position[], float timeArray [], int set, int duration, const int MAX_SIZE)
{
	
	float maxValue = position[0];
	float lowValue = position[0];	

	for (int index = 1; index < MAX_SIZE; index++)
	{
		if (position[index] > maxValue)
		{
			maxValue = position[index];
		}
	}
	
	for (int index = 1; index < MAX_SIZE; index++)
	{
		if (position[index] < lowValue)
		{
			lowValue = position[index];
		}
	}
	
	metafl("PDF"); //To display output on the screen.
  	disini();
  	pagera();
  	complx();
  	chncrv("both");   
  	thkcrv(7);        //Curve thickness
  	axspos(450,1800);
  	axslen(2200,1200);
  	name("Time (s)","x");
  	name("Position (m)","y");
  	labdig(2,"x"); 
  	labdig(6, "y");
  	titlin("Position vs. Time",1);
  	
  	if (set == 0)
  	{
 		titlin("Set 1",3);
 	}
 	
 	else if (set == 1)
 	{
 		titlin("Set 2",3);
	}
	
	else
	{
		titlin("Set 3",3);
	}
 	
  	int ic=intrgb(0.55,0.55,0.55); //the higher the values the brighter background
  	axsbgd(ic);
  	
  	if (set == 0)
  	{
		graf(0.00, duration, 0.00, 1., lowValue, maxValue, lowValue, 0.01);
  	}
	  
	else if (set == 1)
	{
	  	graf(0.00, duration, 0.00, 1, lowValue, maxValue, lowValue, 0.0001);	
	}
	
	else
	{
		graf(0.00, duration, 0.00, 1, lowValue, maxValue, lowValue, 0.00001);
	}	
  	
  	setrgb(0.7,0.7,0.7);
  	grid(1,1);
  	color("fore");
  	height(50);
  	title();

 	curve(timeArray, position, MAX_SIZE);

  	disfin(); //Ends plot session. Must include.
}

	//ALL OBJECT FUNCTIONS

	//systemForce
	void systemForce :: setFrequency (double input)
	{
		frequency = input;
	}
			
	double systemForce :: getFrequency()
	{
		return frequency;
	}
	
	void systemForce :: setTime (double input)
	{
		time = input;
	}
	
	double systemForce :: getTime ()
	{
		return time;
	}
	
	void systemForce :: setforce_initial (double input)
	{
		force_initial = input;
	}
	
	double systemForce :: getforce_initial ()
	{
		return force_initial;
	}
	
	double systemForce :: forceOfTheSystem ()
	{
		force = force_initial * sin(2 * PI * frequency * time); 
	}
	
	//systemSuspension
	void systemSuspension :: setStiffness (double input)
	{
		stiffness = input;
	}
	
	double systemSuspension :: getStiffness()
	{
		return stiffness;
	}
	
	void systemSuspension :: setDamping (double input)
	{
		damping = input;
	}
	
	double systemSuspension :: getDamping ()
	{
		return damping;
	}
	
	void systemSuspension :: setMass (double input)
	{
		mass = input;
	}
	
	double systemSuspension :: getMass ()
	{
		return mass;
	}
	
	double systemSuspension :: getConstantA (double deltaTime)
	{
		constantA =  1.0 / ((mass / (pow(deltaTime, 2))) + (damping/deltaTime));
		
		return constantA;
	}
	
	double systemSuspension :: getConstantB (double deltaTime)
	{
		constantB = (mass / pow(deltaTime, 2));
		
		return constantB;
	}
	
	double systemSuspension :: getConstantC (double deltaTime)
	{
		constantC = (((-2.0 * mass) / pow(deltaTime, 2)) - (damping/deltaTime) + stiffness);
		
		return constantC;
	}
