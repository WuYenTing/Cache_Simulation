#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <tuple>
#include <set>
#include <string>
#include <string.h>
#include <cmath>
#include <algorithm>
#define Min(x,y) (x < y ? x : y)
#define Max(x,y) (x > y ? x : y)
using namespace std;
using LL = long long int;
using block = pair<string,int>;
using bits_data = pair<double,LL>;

LL string_to_int(string str) {
    LL sum = 0;
    LL i = 0;

    while (isdigit(str[i])) {
        sum = sum * 10 + str[i] - '0';
        i++;
    }
    return sum;
}

LL binary_to_decimal(string str) {
    LL sum = 0;
    LL i = 0;

    while (isdigit(str[i])) {
        sum = sum * 2 + str[i] - '0';
        i++;
    }
    return sum;
}


int main(int argc, char* argv[])
{
	ifstream fin;
	fin.open(argv[1], ios::in); //read cache.org

	ofstream fout; 
	fout.open(argv[3], ios::out); //write index.rpt

	string data;
	string Address_bits_string;
	string Block_size_string;
	string Cache_sets_string;
	string Associativity_string;
	string Address_bits_num;
	string Block_size_num;
	string Cache_sets_num;
	string Associativity_num;
	LL Address_bits;
	LL Block_size;
	LL Cache_sets;
	LL Associativity;
	for(int i=0;i<4;i++){
		getline(fin, data,'\n');
		if(data[1]=='d'){
			//Address_bits_string = data;
			size_t found_dash = data.find("_");
			string start = data.substr(0,found_dash);
			string end = data.substr(found_dash+1);
			Address_bits_string = start + " " + end;
			size_t found_space = data.find(" ");
			Address_bits_num = data.substr(found_space+1);
			Address_bits = string_to_int(Address_bits_num);
		}else if(data[1]=='l'){
			//Block_size_string = data;
			size_t found_dash = data.find("_");
			string start = data.substr(0,found_dash);
			string end = data.substr(found_dash+1);
			Block_size_string = start + " " + end;
			size_t found_space = data.find(" ");
			Block_size_num = data.substr(found_space+1);
			Block_size = string_to_int(Block_size_num);
		}else if(data[1]=='a'){
			//Cache_sets_string = data;
			size_t found_dash = data.find("_");
			string start = data.substr(0,found_dash);
			string end = data.substr(found_dash+1);
			Cache_sets_string = start + " " + end;
			size_t found_space = data.find(" ");
			Cache_sets_num = data.substr(found_space+1);
			Cache_sets = string_to_int(Cache_sets_num);
		}else if(data[1]=='s'){
			Associativity_string = data;
			size_t found_space = data.find(" ");
			Associativity_num = data.substr(found_space+1);
			Associativity = string_to_int(Associativity_num);
		}
	}
	fin.close(); //close cache.org

	string reference_start;
	string reference_end;
	vector<string> byte_address;
	LL cache_accesses_count = 0;
	fin.open(argv[2], ios::in); //read reference.lst
	
	while(getline(fin, data,'\n')){
		if (fin.eof()){
			break;
		}else if (data.size()==0){
			continue;
		}else if(data[1]=='b'){
			reference_start = data;
		}else if(data[1]=='e'){
			reference_end = data;
		}else{
			byte_address.push_back(data);
			cache_accesses_count++;
		}
	}
	fin.close();//close reference.lst

	//calculate offset bit and index bit
	LL Offset_bit_count = (LL)log2(Block_size);
	LL Indexing_bit_count = (LL)log2(Cache_sets);
	
	//calculate block address(binary)
	vector<string> block_address;
	LL block_address_bits = Address_bits - Offset_bit_count;
	for(LL i=0;i<cache_accesses_count;i++){
		string tmp_block_address = byte_address[i].substr(0,block_address_bits);
		block_address.push_back(tmp_block_address);
	}

	//calculate address tag(binary) and address index(binary and decimal)
	vector<int> same_position_address_bits[block_address_bits];
    //fout << block_address_bits << "\n";
	for(LL i=0;i<cache_accesses_count;i++){
		for(LL j=0;j<block_address_bits;j++){
			//fout << block_address[i][j] << " ";
			same_position_address_bits[j].push_back(block_address[i][j]-'0');
		}
		//fout << "\n";
	}
	/*
	for(LL i=0;i<block_address_bits;i++){
		for(auto j:same_position_address_bits[i]){
			fout << j << " ";
		}
		fout << "\n";
	}
	*/
	
	
	/*
	for(LL i=0;i<block_address_bits;i++){
		for(LL j=0;j<block_address_bits;j++){
			fout << correlation[i][j] << " ";
		}
		fout << "\n";
	}
	*/
	vector<vector<double>> correlation(block_address_bits,vector<double>(block_address_bits,0));
	double E, D, C;
	for(LL i=0;i<block_address_bits;i++){
		for(LL j=0;j<block_address_bits;j++){
			E = 0;D = 0;C = 0;
			//fout << "i=" << i << ",j=" << j << "  ";
			for(LL k=0;k<cache_accesses_count;k++){
				if(same_position_address_bits[i][k]==same_position_address_bits[j][k]){
					E++;
				}else{
					D++;
				}
			}
			//fout << "E=" << E << ",D=" << D << "\n";
			C = Min(E,D)/Max(E,D);
			//fout << "C=" << C  << "\n";
			correlation[i][j] = C;
			//fout << "correlation[" << i << "][" << j << "]=" << correlation[i][j] << "\n";
		}
	}
	
	vector<bits_data> quality(block_address_bits);
	double Z, O, Q;
	for(LL i=0;i<block_address_bits;i++){
		Z = 0;O = 0;Q = 0;
		for(LL j=0;j<cache_accesses_count;j++){
			if(same_position_address_bits[i][j]==0){
				Z++;
			}else{
				O++;
			}
		}
		//fout << "i=" << i << ", ";
		//fout << "Z=" << Z << ",O=" << O << " ";
		Q = Min(Z,O)/Max(Z,O);
		//fout << "Q=" << Q  << "\n";
		bits_data tmp_bits_data = bits_data(Q, i);
		quality[i]= tmp_bits_data;
		//fout << "quality[" << i << "]=" << get<0>(quality[i]) << " " << get<1>(quality[i])  << "\n";
	}
	
	LL select_bits_count = Indexing_bit_count;
	vector<bits_data> tmp_quality;
	vector<LL> select_bits_array(block_address_bits);
	for(LL i=0;i<block_address_bits;i++){
			tmp_quality.push_back(quality[i]);
	}
	while(select_bits_count){
		set<bits_data> tmp_quality_search;
		for(auto &i:tmp_quality){
			tmp_quality_search.insert(i);
		}

		//fout << "tmp_quality\n";
		/*
		for(auto &i:tmp_quality){
			fout << i.first << " " << i.second << " " << "\n";
		}

		//fout << "tmp_quality_search\n";

		for(auto &i:tmp_quality_search){
			fout << i.first << " " << i.second << " " << "\n";
		}
		*/

		bits_data max = *(tmp_quality_search.rbegin());
		//fout << "max\n";
		//fout << max.first << " " << max.second << " " << "\n";
		LL max_index = max.second;
		select_bits_array[max_index]=1;

		//fout << "select_bits_array\n";
		/*
		for(auto &i:select_bits_array){
			fout << i << " ";   
		}
		fout << "\n";
		*/
		vector<bits_data>::iterator tmp_max_it;
		for(auto it=tmp_quality.begin();it!=tmp_quality.end();it++){
			if(*it==max){
				tmp_max_it = it;
			}
		}
		tmp_quality.erase(tmp_max_it);

		/*
		fout << "before cal\n";

		for(auto &i: tmp_quality){
			fout << i.first << " " << i.second << "\n";   
		}
		fout << "\n";
		*/
		for(auto &i : tmp_quality){
			i.first *= correlation[i.second][max_index];
		}
		/*
		fout << "after cal\n";

		for(auto &i: tmp_quality){
			fout << i.first << " " << i.second << "\n";   
		}
		fout << "\n";
		*/
		select_bits_count--;
	}
	
	vector<string> address_tag_binary(cache_accesses_count);   //沒標記挑選的block address,由左到右push進去
	vector<string> address_index_binary(cache_accesses_count); //有標記挑選的block address,由左到右push進去
	
	LL address_tag_bits = block_address_bits - Indexing_bit_count;
	for(LL i=0;i<cache_accesses_count;i++){
		for(LL j=0;j<block_address_bits;j++){
			char ch = block_address[i][j];
			if(select_bits_array[j]==1){
				//fout << "index_bits=" << ch << " ";
				address_index_binary[i].push_back(ch);    
			}else{
				//fout << "tag_bits=" << ch << " ";
				address_tag_binary[i].push_back(ch);  
			}
			//fout << "\n";
		}
	}
	vector<LL> address_index_decimal;    //拿address_index_binary轉換
	for(LL i=0;i<cache_accesses_count;i++){
		LL tmp_index_decimal = binary_to_decimal(address_index_binary[i]);
		address_index_decimal.push_back(tmp_index_decimal);
	}
	/*
	fout << "\naddress_index_binary\n";

	for(LL i=0;i<cache_accesses_count;i++){
		for(auto j:address_index_binary[i]){
			fout << j;
		}
		fout << "\n";
	}

	fout << "\naddress_tag_binary\n";

	for(LL i=0;i<cache_accesses_count;i++){
		for(auto j:address_tag_binary[i]){
			fout << j;
		}
		fout << "\n";
	}
	*/



	vector<LL> Indexing_bits; //把標記挑選的block address編號(從右邊往左數),由左到右push進去
	for(LL i=0;i<block_address_bits;i++){
		if(select_bits_array[i]==1){
			Indexing_bits.push_back(Address_bits-1-i);
		}
	}
	/*
	fout << "Indexing_bits:";

	for(auto &i:Indexing_bits){
		fout << i << " ";
	}
	fout << "\n";
	*/

	
	//create cache_sets
	vector<block> cache_sets[Cache_sets];
	vector<string> hit_miss_result;
	LL total_cache_miss_count = 0;
	for(LL i=0;i<cache_accesses_count;i++){
		LL current_index_set_size = cache_sets[address_index_decimal[i]].size();
		//假如對應index是空的,直接insert pair<tag,NRU bit=1>,且為cache miss
		if(current_index_set_size==0){
			block tmp_block = pair<string,int>(address_tag_binary[i],0);
			cache_sets[address_index_decimal[i]].push_back(tmp_block);
			hit_miss_result.push_back("miss");
			total_cache_miss_count++;
		}else{
			//假如對應index已裝滿
			//檢查目前有的元素的NRU bit是否有1
			//檢查目前有的元素tag,是否有跟access的相同
			int tag_find = 0;
			int nru_bit_1_check = 0;
			
			for(auto &b : cache_sets[address_index_decimal[i]]){
				if(b.first==address_tag_binary[i] == 1){
					tag_find = 1;
				}
			}
			if(tag_find == 1){
				for(auto &b : cache_sets[address_index_decimal[i]]){
					if(b.first==address_tag_binary[i]){
						b.second = 0;
						hit_miss_result.push_back("hit");
						break;
					}
				}
			}else{
				for(auto &b : cache_sets[address_index_decimal[i]]){
					if(b.second==1){
						nru_bit_1_check = 1;
					}
				}
				if(nru_bit_1_check == 1){
					LL tmp_nru_bit_1_block_index;
					for(LL j=0;j<cache_sets[address_index_decimal[i]].size();j++){
						if(cache_sets[address_index_decimal[i]][j].second == 1 ){
							tmp_nru_bit_1_block_index = j;
							break;
						}
					}
					vector<block>::iterator tmp_nru_bit_1_block_it;
					for(auto it=cache_sets[address_index_decimal[i]].begin();it!=cache_sets[address_index_decimal[i]].end();it++){
						if( (*it).second == 1){
							tmp_nru_bit_1_block_it = it;
							break;
						}
					}
					block tmp_block = pair<string,int>(address_tag_binary[i],0);
					if(current_index_set_size < Associativity){
						cache_sets[address_index_decimal[i]].insert(tmp_nru_bit_1_block_it,tmp_block);
					}else if(current_index_set_size == Associativity){
						cache_sets[address_index_decimal[i]][tmp_nru_bit_1_block_index] = tmp_block;
					}
					hit_miss_result.push_back("miss");
					total_cache_miss_count++;
				}else{
					for(auto &b : cache_sets[address_index_decimal[i]])
					{
						b.second = 1;	
					}
					block tmp_block = pair<string,int>(address_tag_binary[i],0);
					if(current_index_set_size < Associativity){
						cache_sets[address_index_decimal[i]].insert(cache_sets[address_index_decimal[i]].begin(),tmp_block);
					}else if(current_index_set_size == Associativity){
						cache_sets[address_index_decimal[i]].erase(cache_sets[address_index_decimal[i]].begin());
						cache_sets[address_index_decimal[i]].insert(cache_sets[address_index_decimal[i]].begin(),tmp_block);
					}
					hit_miss_result.push_back("miss");
					total_cache_miss_count++;
				}
			}

			//fout << "\n";
			//fout << "nru_bit_1_check= " << nru_bit_1_check << "\n";
			//fout << "tag_find= " << tag_find << "\n";

		}	
		/*
		fout << address_index_decimal[i] << " ";
		
		for(auto b : cache_sets[address_index_decimal[i]]){
			fout << b.first << " " << b.second << " ";
		}
		fout << "\n";
		*/
	}
	
	//fout << "\n";

	//output
	fout << Address_bits_string << "\n";
	fout << Cache_sets_string << "\n";
	fout << Associativity_string << "\n";
	fout << Block_size_string << "\n";
	fout << "\n";
	
	//fout << "Address_bits=" << Address_bits << "\n";
	//fout << "Cache_sets=" << Cache_sets << "\n";
	//fout << "Associativity=" << Associativity << "\n";
	//fout << "Block_size=" << Block_size << "\n";
	
	//fout << "\n";

	fout << "Indexing bit count: " << Indexing_bit_count << "\n";
	fout << "Indexing bits: ";
	if(Indexing_bit_count==0){
		fout << "\n";
	}else{
		for(LL i=0;i<Indexing_bit_count;i++){
			fout << Indexing_bits[i] << ( (i==Indexing_bit_count-1) ? "\n" : " ");
		}
	}

	fout << "Offset bit count: " << Offset_bit_count << "\n";
	fout << "\n";

	fout << reference_start << "\n";
	for(LL i=0;i<cache_accesses_count;i++){
		fout << byte_address[i] << " " << hit_miss_result[i] << "\n";
	}
	fout << reference_end << "\n" ;
	fout << "\n";

	fout << "Total cache miss count: " << total_cache_miss_count << "\n";
	
	/*
	for(auto i:block_address){
		fout << i <<"\n";
	}

	for(auto i:address_tag_binary){
		fout << i <<"\n";
	}

	for(auto i:address_index_binary){
		fout << i <<"\n";
	}

	for(auto i:address_index_decimal){
		fout << i <<"\n";
	}

	for(auto i:hit_miss_result){
		fout << i <<"\n";
	}

	for(auto i:Indexing_bits){
		fout << i <<"\n";
	}
	*/

	fout.close(); //close index.rpt

	return 0;

}