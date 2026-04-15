#include<algorithm>	//find()
#include<cstdlib>	//RAND_MAX 정의
#include<stdexcept>	//dimain_error,	...
#include<ctime>		//time()...
//time()의 반환 값은 1970년1월1일 00:00부터~~~ 지금까지 건너뛴 시간(ms)=1675430097
#include<fstream>	//파일을 읽기 위해...
#include<iostream>
#include<vector>
#include<string>
#include<map>

#include "split_str.h"

using namespace std;

typedef vector<string>Rule;
typedef vector<Rule> Rule_collection;	//vector<vector<string>>
typedef map<string, Rule_collection>Grammar;
//map<string(키), vector<vector<string>>(값)>
typedef map<string, string> xref;

string line_info;

//주어진 입력 스트림으로 문법 테이블 읽음
Grammar read_grammar(const string& filename) {
	// aaron.kr에 잇는 grammar.txt나 grammar long.txt필수
	Grammar ret;
	string line;

	ifstream grammar_file(filename);	//에런의 수정한 부분...

	if (!grammar_file) {
		cerr << "Error opening file:	" << filename << endl;
		throw runtime_error("Failed to open file.");
	}

	//입력 데이터를 읽음
	while (getline(grammar_file, line)) {
		//입력데이터를 단어로 나눔
		vector<string>entry = lib_split(line);
		if (!entry.empty()) {
			//키테고리 및 연관된 규칙을 맵에 저장
			ret[entry[0]].push_back(
				Rule(entry.begin() + 1, entry.end())
			);
		}
	}
	return ret;
}

//꺽쇠호<> 있는 지확인 함수
bool bracketed(const string& s) {
	//<....>?true:false
	return s.size() > 1 && s[0] == '<' && s[s.size() - 1] == '>';
}

//[0,n) 범위에서 임의 정수 하나르 반환
int nrand(int n) {	//PYTHON의 random 모듈에서 같은 함수있다
	if (n <= 0 || n > RAND_MAX)
		throw domain_error("nrand out of range");
	const int bucket_size = RAND_MAX / n;
	int r;
	do r = rand() / bucket_size;	//무조건 한 번 이상 실행
	while (r >= n);
	/*
	do{
		r=rand()/ bucket_size;
	}while(r>=n);
	*/
	return r;
}

//문법 사용하고 문장 생성하기
void gen_aux(const Grammar& g, const string& word,
	vector<string>& ret) {
	if (!bracketed(word))ret.push_back(word);

	else {//<...>있는 단어(문법규칙)
		//word과 연관된 규칙을 찾음
		Grammar::const_iterator it = g.find(word);

		if (it == g.end())	//벌써 끝??
			throw logic_error("Empty rule!");

		//선택 가능한 규칙을 모아 놓은 벡터를 가져옴
		const Rule_collection& c = it->second;

		//임의로 하나를 선택
		const Rule& r = c[nrand(c.size())];	//난수 선택

		//선택한 규칙을 재귀적으로 확장
		for (Rule::const_iterator i = r.begin();
			i != r.end(); ++i) {
			gen_aux(g, *i, ret);
		}

	}
}

//일반적으로 문장 생성하기
vector<string> gen_sent(const Grammar& g) {
	vector<string>ret;
	gen_aux(g, "<sentence>", ret);
	return ret;
}

map<string, string> xref(istream& in, vector<string> find_words(const string&) = lib_split)
{
	string line;
	int line_num = 0;

	map<string, vector<int>> temp;
	map<string, string> result;

	while (getline(in, line)) {
		++line_num;

		vector<string> words = find_words(line);

		for (vector<string>::const_iterator it = words.begin();
			it != words.end(); ++it) {

			temp[*it].push_back(line_num);
		}
	}

	// 👉 line / lines 변환
	for (map<string, vector<int>>::const_iterator it = temp.begin();
		it != temp.end(); ++it) {

		ostringstream oss();

		if (it->second.size() == 1) {
			line_info = "line ";
			line_info += to_string(it->second[0]);
		}
		else {
			line_info = "lines ";

			for (size_t i = 0; i < it->second.size(); ++i) {
				if (i > 0) line_info += ", ";
				line_info += to_string(it->second[i]);
			}
		}
	}

	return result;
}

//MAINNNNNNNNNNNNNNNNNNNNNN
int main(void) {
	map<string, string> ret = xref(cin);

	for (map<string, string>::const_iterator it = ret.begin();
		it != ret.end(); ++it) {

		cout << "\"" << it->first << "\" is on " << it->second << endl;
	}
	return 0;
}
