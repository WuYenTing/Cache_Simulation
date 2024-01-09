#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <string.h>
#include <cmath>
#include <algorithm>
using namespace std;
using LL = long long int;
using block = pair<string,int>;

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
	//read cache.org
	ifstream fin;
	fin.open(argv[1], ios::in);   

	//write index.rpt
	ofstream fout; 
	fout.open(argv[3], ios::out); 

	//get 4 parameters(Address_bits,Block_size,Cache_sets,Associativity)
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
	//close cache.org
	fin.close(); 

	string reference_start;
	string reference_end;
	vector<string> byte_address;
	LL cache_accesses_count = 0;
	//read reference.lst
	fin.open(argv[2], ios::in); 
	//get byte address data
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
	//close reference.lst
	fin.close();

	//calculate offset bit and index bit
	LL Offset_bit_count = (LL)log2(Block_size);
	LL Indexing_bit_count = (LL)log2(Cache_sets);
	
	//calculate block address(binary) & block_address_bits
	vector<string> block_address;
	LL block_address_bits = Address_bits - Offset_bit_count;
	for(int i=0;i<cache_accesses_count;i++){
		string tmp_block_address = byte_address[i].substr(0,block_address_bits);
		block_address.push_back(tmp_block_address);
	}

	//calculate address tag(binary) and address index(binary and decimal)
	vector<string> address_tag_binary;
	vector<string> address_index_binary;
	vector<LL> address_index_decimal;
	LL address_tag_bits = block_address_bits - Indexing_bit_count;
	for(LL i=0;i<cache_accesses_count;i++){
		string tmp_tag = block_address[i].substr(0,address_tag_bits);
		address_tag_binary.push_back(tmp_tag);
		string tmp_index = block_address[i].substr(address_tag_bits,Indexing_bit_count);
		address_index_binary.push_back(tmp_index);
		LL tmp_index_decimal = binary_to_decimal(tmp_index);
		address_index_decimal.push_back(tmp_index_decimal);
	}

	//calculate Indexing_bits(binary)
	vector<LL> Indexing_bits;
	for(LL i=0;i<Indexing_bit_count;i++){
		Indexing_bits.push_back(i+Offset_bit_count);
	}
	sort(Indexing_bits.rbegin(),Indexing_bits.rend());

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
