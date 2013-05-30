// Custom TOPSIS (Technique for Order Preference by Similarity to Ideal Solution) for torrent recommandation /////////////////////////////
// By: Arnaud Benard for: movies.io																										//
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include "armadillo"
#include <time.h>

#define N_TORRENT 100000
#define N_CRITERIA 3
#define N_QUALITY_KEYWORDS 4
#define N_LANGUAGE_KEYWORDS 4
#define N_RELEASE_KEYWORDS 5

using namespace std;
using namespace arma;

int analyze_decision_matrix(string torrent_name, int criteria_number);
bool find_word_torrent_name (string torrent_name, string keyword);
double calculate_norm(rowvec torrent_spec);
string* generate_torrent_list(string* torrent);


int main() {
 
	int i,j; 					// int for the loops

	// These variables are only relevant for our example
	// Criteria (4) : 
	// Quality
	// Movie size
	// Language
	// Release group

	// 2 torrents
	string* torrent = new string[N_TORRENT];
	torrent=generate_torrent_list(torrent);

	// Decision matrix 
	mat decision_matrix(N_TORRENT,N_CRITERIA); // float because we'll have to normalize the matrix -> matrix[i][j] E [0;1]

	// Weight vector: importance of criteria 
	int quality_w = 3,
	size_w=2,
	language_w=10,
	release_group_w=1;

	// Init weight_vector
	colvec weight_vector;
	weight_vector << quality_w << size_w << language_w << release_group_w; 

	//Building the decision matrix
	for(i=0;i<N_TORRENT;i++){
		for(j=0;j<N_CRITERIA;j++){
			decision_matrix(i,j)=analyze_decision_matrix(torrent[i],j); // This function will benchmark the torrent according to the j-criteria and return the decision matrix
		}
	}
	// Normalize Matrix + Multiply by weight
	vec norm(N_TORRENT);

	for(int k=0;k<N_TORRENT;k++){
		norm(k)=calculate_norm(decision_matrix.row(k));
	
		for(int l=0;l<N_CRITERIA;l++){	
			decision_matrix(k,l)=decision_matrix(k,l)/norm(k)*weight_vector(l);		 		
		}
	}
	//Finds max
	rowvec max_vector = max(decision_matrix,0);
	rowvec min_vector = min(decision_matrix,0);

	//Positive/negative distances
	vec positive_ideal(N_TORRENT);
	vec negative_ideal(N_TORRENT);

	double sum_positive=0, sum_negative=0;

	for (int m = 0; m < N_TORRENT; m++)
	{
		for (int n = 0; n < N_CRITERIA; n++)
			{
				sum_positive+=pow(decision_matrix(m,n)-max_vector(n),2);
				sum_negative+=pow(decision_matrix(m,n)-min_vector(n),2);
			}
			positive_ideal(m)=sqrt(sum_positive);
			negative_ideal(m)=sqrt(sum_negative);

			sum_positive=0;
			sum_negative=0;	
	}
	vec final_ranking(N_TORRENT);
	for (int o = 0; o < N_TORRENT; o++)
	{
		final_ranking(o)=(negative_ideal(o))/(negative_ideal(o)+positive_ideal(o));
	}

	return 0;

}

int analyze_decision_matrix(string torrent_name, int criteria_number){

	int quality_counter=0;
	int language_counter=0;
	int release_counter=0;


	// Boring definitions
	//Quality case 0
	colvec quality_points;
	quality_points.fill(0.0);
	quality_points << 3 << 3 << 5 << 2;
	string* quality_keywords= new string[N_QUALITY_KEYWORDS];
	quality_keywords[0]="h264";
	quality_keywords[1]="x264";
	quality_keywords[2]="mkv";
	quality_keywords[3]="r5";

	// Language case 1
	colvec language_points;
	language_points.fill(0.0);
	language_points << 3 << 3 << 3 << 1;
	string* language_keywords= new string[N_LANGUAGE_KEYWORDS];
	language_keywords[0]="en";
	language_keywords[1]="eng";
	language_keywords[2]="english";
	language_keywords[3]="fr";

	// Release group case 2
	colvec release_points;
	release_points.fill(0.0);
	release_points << 3 << 3 << 5 << 2 << 5;
	string* release_keywords= new string[N_RELEASE_KEYWORDS];
	release_keywords[0]="sinners";
	release_keywords[1]="kingdom";
	release_keywords[2]="axxo";
	release_keywords[3]="max";
	release_keywords[4]="proper";

    switch(criteria_number){

    	case 0 : 
    	// Process for quality

    	for (int i = 0; i < N_QUALITY_KEYWORDS; i++)
    	{	
    		
			if(find_word_torrent_name(torrent_name,quality_keywords[i]))
			{
				quality_counter+=quality_points(i);
			}
    	}
    	if(quality_counter!=0){
    		return quality_counter;
    	}
    	else{
    		return 1;
    	}
    	break;

		case 1 : 
    	// Process for language

    	for (int j = 0; j < N_LANGUAGE_KEYWORDS; j++)
    	{	
			if(find_word_torrent_name(torrent_name,language_keywords[j]))
			{
				language_counter+=language_points(j);
			}
    	}
    	if(language_counter!=0){
    		return language_counter;
    	}
    	else{
    		return 1;
    	}
    	break;
    	case 2 : 
    	// Process for language

    	for (int k = 0; k < N_RELEASE_KEYWORDS; k++)
    	{	
    		if(find_word_torrent_name(torrent_name,release_keywords[k])){
				release_counter+=release_points(k);
			}
		}
    	if(release_counter!=0){
    		return release_counter;
    	}
    	else{
    		return 1;
    	}
    	break;

    }

	return 0;
}
bool find_word_torrent_name (string torrent_name, string keyword){

	if (torrent_name.find (keyword) != string::npos)
		return 1;
	
	else
		return 0;   	
}
double calculate_norm(rowvec torrent_spec){

	double norm=0;

	for (int i = 0; i < N_CRITERIA; ++i)
	{
		norm+=(pow(torrent_spec(i),2));
	}
	return norm;
}
string* generate_torrent_list(string* torrent){

	for (int r = 0; r < N_TORRENT; r++)
	{
		torrent[r] = "Inception 2010 BluRay 1080p VC1 DTS HD MA5 1 CHDBits max mkv axxo en";
	}


	return torrent;

}
