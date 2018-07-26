#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <iterator>

using namespace std;

// Structure containing drug name, total cost 
// and set of prescribers first and last name
struct DrugInfo
{
	string DrugName;
	double TotalCost;
	unordered_set<string> Prescribers;
};

class DrugList
{
private:
	unordered_map<string, DrugInfo> drug_map;
	unordered_map<string, DrugInfo>::const_iterator got;
	vector<DrugInfo> drug_vec;
public:

	//Function to create a drug info map
	void DrugInfo_map(string drugName, string p_firstname, string p_lastname, double cost)
	{
		got = drug_map.find(drugName);
		//For adding new drug
		if (got == drug_map.end())
		{
			drug_map[drugName].DrugName = drugName;
			drug_map[drugName].TotalCost = cost;
			drug_map[drugName].Prescribers.insert(p_firstname + "_" + p_lastname);
		}
		//For adding info to an existing drug
		else
		{
			drug_map[drugName].TotalCost += cost;
			drug_map[drugName].Prescribers.insert(p_firstname + "_" + p_lastname);
		}
	}

	// Function to parse input file into a map where key is drug name
	// and the value is drug info. This code always assumes format of file as
	// id,prescriber_last_name,prescriber_first_name,drug_name,drug_cost
	void ReadDatafile(string InputFileName)
	{
		string id, pres_last_name, pres_first_name, drugName, cost;
		double Cost;
		
		ifstream myfile(InputFileName);
		if (!myfile.is_open())
		{
			cout << "Error Opening File" << "\n";
		}
		else
		{
			int i = 0;
			while (!myfile.fail())
			{
				getline(myfile, id, ',');
				getline(myfile, pres_last_name, ',');
				getline(myfile, pres_first_name, ',');
				getline(myfile, drugName, ',');
				getline(myfile, cost, '\n');
				if (id == "") break;
				if (i != 0)
				{
					try
					{
						Cost = stod(cost);
					}
					catch (const invalid_argument)
					{
						std::cerr << "Invalid data on line " << i + 1 << ". Ignoring\n";
						++i;
						continue;
					}
					DrugInfo_map(drugName, pres_first_name, pres_last_name, Cost);
				}
				if (myfile.eof()) break;
				++i;
			}
			myfile.close();
		}

	}

	//Function to copy DrugInfo map to a vector which will be
	//used for sorting
	void copy_map_to_vec()
	{
		for (auto it = drug_map.begin(); it != drug_map.end(); ++it)
		{
			drug_vec.push_back(it->second);
		}
	}

	int GetPivot(int start, int end)
	{
		DrugInfo pivot = drug_vec[end];
		int index = start - 1;
		for (int i = start; i <= end - 1; i++)
		{
			bool swap = false;
			if (drug_vec[i].TotalCost > pivot.TotalCost)
			{
				swap = true;
			}
			else if ((drug_vec[i].TotalCost == pivot.TotalCost) &&
				(drug_vec[i].DrugName == pivot.DrugName))
			{
				swap = true;
			}

			if (swap)
			{
				index++;
				DrugInfo *temp = &drug_vec[index];
				drug_vec[index] = drug_vec[i];
				drug_vec[i] = *temp;
			}
		}

		// Swap the pivot
		index++;
		drug_vec[end] = drug_vec[index];
		drug_vec[index] = pivot;

		return index;
	}

	// Quicksort algorithm to sort drug_vec based on total cost and drug name
	void SortDrugInfo()
	{
		int start = 0;
		int end = drug_vec.size() - 1;
		if (start > end)
		{
			return;
		}

		int pivot = GetPivot(start, end);
		SortDrugInfo(start, pivot - 1);
		SortDrugInfo(pivot + 1, end);
	}

	void SortDrugInfo(int start, int end)
	{
		if (start > end)
		{
			return;
		}

		int pivot = GetPivot(start, end);
		SortDrugInfo(start, pivot - 1);
		SortDrugInfo(pivot + 1, end);
	}

	// Function to Write out data passed in drug_vec to disk
	void WriteOutDataFile(string outputFileName)
	{
		ofstream myfile(outputFileName);
		if (myfile.is_open())
		{
			myfile << "drug_name,num_prescriber,total_cost\n";
			for (auto drugInfo : drug_vec)
			{
				myfile << drugInfo.DrugName << "," << drugInfo.Prescribers.size() << "," << drugInfo.TotalCost << endl;
			}
			myfile.close();
		}
		else cout << "Unable to open file";
	}
};

int main(int argc, char *argv[])
{
	DrugList drug;

	if (argc < 3 || argc >3)
	{
		cout << "Invalid number of arguments passed\n";
		cout << "Usage: <path to input file> <path to output file>";
	}

	try
	{
		// Read input file
		drug.ReadDatafile(argv[1]);

		// Copy druginfo map to vector 
		drug.copy_map_to_vec();

		// Sort all the drugs based on total cost
		drug.SortDrugInfo();

		// Write output to disk
		drug.WriteOutDataFile(argv[2]);
	}
	catch (const std::exception &e)
	{
		cout << e.what() << endl;
	}
}
