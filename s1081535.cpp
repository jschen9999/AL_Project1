#include<iostream>
#include<string>
#include<sstream>
using namespace std;

#include<fstream>
using std::ifstream;
using std::ofstream;
ifstream infile("test.txt", ios::in);
ofstream outfile("ans.txt", ios::out);

string inst_s[10000];
int i_num = 0;
int i_current;
//store location of lebal
string label_name[500];
int label_loc[500];
int label_num = 0;
void check_label(string s) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == ':') {
			for (int j = 0; j < i; j++) {
				if(s[j]!=' ')label_name[label_num] += s[j];
			}
			inst_s[i_num] = "";
			for (int j = i; j < s.length(); j++) {
				if ((s[j] != ':' &&s[j] != ' ')|| inst_s[i_num] != "") {
					inst_s[i_num] += s[j];
				}
			}
			label_loc[label_num] = i_num;
			label_num++;
			break;
		}
		if (i == s.length() - 1) {
			inst_s[i_num] = "";
			for (int j = 0; j < s.length(); j++) {
				//not equal to space&tab
				if ((s[j] != ' '&&s[j] != '	') || inst_s[i_num] != "") {
					inst_s[i_num] += s[j];
				}
			}
			break;
		}
	}
}

//instruction act
string act[10000];
void cut(string s) {
	for (int j = 0; s[j] != ' '; j++) {
		if (s[j] != ' ') {
			act[i_num] += s[j];
		}
	}
}

//to binary
string binary(string s) {
	int n;
	string r;
	stringstream ss;
	ss << s;
	ss >> n;
	while (n != 0) {
		r = (n % 2 == 0 ? "0" : "1") + r;
		n /= 2;
	}
	if (r == "")return s;
	else return r;
}

//fix to correct bits
string fix(string s, int n) {
	while (s.length() != n) {
		s = "0" + s;
	}
	return s;
}

//find label address offset
string find_address(string s) {
	for (int j = 0; j < label_num; j++) {
		if (label_name[j] == s) {
			//outfile << label_name[j] << endl;
			//outfile << label_loc[j] << endl;
			stringstream sss;
			string loc_num;
			int loc_numi = label_loc[j] - i_current;
			sss << loc_numi;
			sss >> loc_num;
			return loc_num;
		}
	}
}

//getvalue of R-type instruction
string func7_r = "0000000";
string opcode_r = "0110011";
string rs1_r, rs2_r, rd_r, func3_r;
void getvalue_r(string s) {
	int reg_num = 0;
	for (int j = 0; reg_num < 3 && j < s.length(); j++) {
		if (s[j] == 'x' || s[j] == 'X') {
			int k;
			for (k = j + 1; s[k] != ','&&k < s.length(); k++) {
				if (reg_num == 0)rd_r += s[k];
				else if (reg_num == 1)rs1_r += s[k];
				else rs2_r += s[k];
			}
			j = k;
			reg_num++;
		}
	}
	rs1_r = fix(binary(rs1_r), 5);
	rs2_r = fix(binary(rs2_r), 5);
	rd_r = fix(binary(rd_r), 5);
	//output answer of R-type
	outfile << func7_r << rs2_r << rs1_r << func3_r << rd_r << opcode_r << endl;
	//reset
	func7_r = "0000000";
	opcode_r = "0110011";
	rs1_r = "";
	rs2_r = "";
	rd_r = "";
	func3_r = "";
}

//getvalue of S-type instruction
string opcode_s = "0100011";
string rs1_s, rs2_s, func3_s, imm1_s, imm2_s;
void getvalue_s(string s) {
	int s_num = 0;
	int k;
	string offset_s;
	for (int j = 0; s_num < 2 && j < s.length(); j++) {
		if (s[j] == 'x' || s[j] == 'X') {
			for (k = j + 1; s[k] != ','&&k < s.length(); k++) {
				if (s_num == 1)rs1_s += s[k];
				else rs2_s += s[k];
			}
			j = k;
			s_num++;
		}
		if (s[j] == '(') {
			for (int m = k + 1; m < j; m++) {
				if (s[m] != ' '&&s[m] != ',')offset_s += s[m];
			}
			//outfile << offset_s << endl;
		}

	}
	rs1_s = fix(binary(rs1_s), 5);
	rs2_s = fix(binary(rs2_s), 5);
	if (offset_s[0] == '-') {
		stringstream chs;
		int chsi;
		chs << offset_s;
		chs >> chsi;
		chsi += 4096;
		stringstream res;
		res << chsi;
		res >> offset_s;
	}
	offset_s = fix(binary(offset_s), 12);
	for (int j = 0; j < 12; j++) {
		if (j < 7) imm1_s += offset_s[j];
		else imm2_s += offset_s[j];
	}
	//output answer of S-type
	outfile << imm1_s << rs2_s << rs1_s << func3_s << imm2_s << opcode_s << endl;
	//reset
	opcode_s = "0100011";
	rs1_s = "";
	rs2_s = "";
	func3_s = "";
	imm1_s = "";
	imm2_s = "";
}

// getvalue of I - type (offset)instruction
string opcode_io = "0000011";
string rs1_io, rd_io, func3_io;
void getvalue_io(string s) {
	int io_num = 0;
	int k;
	string offset_io;
	for (int j = 0; io_num < 2 && j < s.length(); j++) {
		if (s[j] == 'x' || s[j] == 'X') {
			for (k = j + 1; s[k] != ','&&k < s.length(); k++) {
				if (io_num == 1)rs1_io += s[k];
				else rd_io += s[k];
			}
			j = k;
			io_num++;
		}
		if (s[j] == '(') {
			for (int m = k + 1; m < j; m++) {
				if (s[m] != ' '&&s[m] != ',')offset_io += s[m];
			}
			//outfile << offset_s << endl;
		}

	}
	rs1_io = fix(binary(rs1_io), 5);
	rd_io = fix(binary(rd_io), 5);
	if (offset_io[0] == '-') {
		stringstream chio;
		int chioi;
		chio << offset_io;
		chio >> chioi;
		chioi += 4096;
		stringstream reio;
		reio << chioi;
		reio >> offset_io;
	}
	offset_io = fix(binary(offset_io), 12);
	//output answer of I-type (offset)
	outfile << offset_io << rs1_io << func3_io << rd_io << opcode_io << endl;
	//reset
	opcode_io = "0000011";
	rs1_io = "";
	rd_io = "";
	func3_io = "";
}

// getvalue of I - type (immediate)instruction
string opcode_ii = "0010011";
string rs1_ii, rd_ii, func3_ii, imm_ii;
void getvalue_ii(string s) {
	int ii_num = 0;
	int k = 0;
	for (int j = 0; ii_num < 2 && j < s.length(); j++) {
		if (s[j] == 'x' || s[j] == 'X') {
			for (k = j + 1; s[k] != ','&&k < s.length(); k++) {
				if (ii_num == 1)rs1_ii += s[k];
				else rd_ii += s[k];
			}
			j = k;
			ii_num++;
		}
		if (ii_num == 2) {
			for (int m = j; m < s.length(); m++) {
				if (s[m] != ' '&&s[m] != ',')imm_ii += s[m];
			}
			//outfile << imm_ii << endl;
		}

	}
	rs1_ii = fix(binary(rs1_ii), 5);
	rd_ii = fix(binary(rd_ii), 5);
	if (imm_ii[0] == '-') {
		stringstream chii;
		int chiii;
		chii << imm_ii;
		chii >> chiii;
		chiii += 4096;
		stringstream reii;
		reii << chiii;
		reii >> imm_ii;
	}
	imm_ii = fix(binary(imm_ii), 12);
	//output answer of I-type (immediate)
	outfile << imm_ii << rs1_ii << func3_ii << rd_ii << opcode_ii << endl;
	//reset
	opcode_ii = "0010011";
	rs1_ii = "";
	rd_ii = "";
	func3_ii = "";
	imm_ii = "";
}

// getvalue of R - type (rv64i)instruction
string opcode_rup = "0010011";
string func6_rup = "000000";
string rs1_rup, rd_rup, func3_rup, imm_rup;
void getvalue_rup(string s) {
	int rup_num = 0;
	int k = 0;
	for (int j = 0; rup_num < 2 && j < s.length(); j++) {
		if (s[j] == 'x' || s[j] == 'X') {
			for (k = j + 1; s[k] != ','&&k < s.length(); k++) {
				if (rup_num == 1)rs1_rup += s[k];
				else rd_rup += s[k];
			}
			j = k;
			rup_num++;
		}
		if (rup_num == 2) {
			for (int m = j; m < s.length(); m++) {
				if (s[m] != ' '&&s[m] != ',')imm_rup += s[m];
			}
			//outfile << imm_ii << endl;
		}

	}
	rs1_rup = fix(binary(rs1_rup), 5);
	rd_rup = fix(binary(rd_rup), 5);
	imm_rup = fix(binary(imm_rup), 6);
	//output answer of R(64i)-type
	outfile << func6_rup << imm_rup << rs1_rup << func3_rup << rd_rup << opcode_rup << endl;
	//reset
	opcode_rup = "0010011";
	func6_rup = "000000";
	rs1_rup = "";
	rd_rup = "";
	func3_rup = "";
	imm_rup = "";
}

//getvalue of SB-type instruction
string opcode_sb = "1100011";
string rs1_sb, rs2_sb, imm1_sb, imm2_sb, func3_sb;
void getvalue_sb(string s) {
	int sb_num = 0;
	int k;
	string offset_sb;
	for (int j = 0; sb_num < 2 && j < s.length(); j++) {
		if (s[j] == 'x' || s[j] == 'X') {
			for (k = j + 1; s[k] != ','&&k < s.length(); k++) {
				if (sb_num == 1)rs2_sb += s[k];
				else rs1_sb += s[k];
			}
			j = k;
			sb_num++;
		}
		if (sb_num == 2) {
			for (int m = j; m < s.length(); m++) {
				if (s[m] != ' '&&s[m] != ',')offset_sb += s[m];
			}
			//outfile << offset_sb << endl;
		}

	}
	offset_sb = find_address(offset_sb);
	if (offset_sb[0] == '-') {
		stringstream chsb;
		int chisb;
		chsb << offset_sb;
		chsb >> chisb;
		chisb += 2048;
		stringstream resb;
		resb << chisb;
		resb >> offset_sb;
	}
	//outfile << offset_sb << endl;
	rs1_sb = fix(binary(rs1_sb), 5);
	rs2_sb = fix(binary(rs2_sb), 5);
	offset_sb = fix(binary(offset_sb), 11) + "0";
	for (int j = 0; j < 12; j++) {
		if (j == 0 || (j > 1 && j < 8)) imm1_sb += offset_sb[j];
		else if (j != 1)imm2_sb += offset_sb[j];
	}
	imm2_sb += offset_sb[1];
	//output answer of SB-type
	outfile << imm1_sb << rs2_sb << rs1_sb << func3_sb << imm2_sb << opcode_sb << endl;
	//reset
	opcode_sb = "1100011";
	rs1_sb = "";
	rs2_sb = "";
	func3_sb = "";
	imm1_sb = "";
	imm2_sb = "";
}

// getvalue of U - type instruction
string opcode_u ;
string rd_u, imm_u;
void getvalue_u(string s) {	
	for (int j = 0; j < s.length(); j++) {
		if (s[j] == 'x' || s[j] == 'X') {
			int k = 0;
			for (k = j + 1; s[k] != ','&&k < s.length(); k++) {
				rd_u += s[k];
			}
			for (int m = k; m < s.length(); m++) {
				if (s[m] != ' '&&s[m] != ',')imm_u += s[m];
			}
			break;
		}
	}
	rd_u = fix(binary(rd_u), 5);
	if (imm_u[0] == '-') {
		stringstream chu;
		int chiu;
		chu << imm_u;
		chu >> chiu;
		chiu += 1048576;
		stringstream reu;
		reu << chiu;
		reu >> imm_u;
	}
	imm_u = fix(binary(imm_u), 20);
	//output answer of U-type
	outfile << imm_u << rd_u << opcode_u << endl;
	//reset
	opcode_u = "";
	rd_u = "";
	imm_u = "";
}

// getvalue of UJ - type instruction
string opcode_uj;
string rd_uj, offset_uj;
void getvalue_uj(string s) {	
	
	for (int j = 0; j < s.length(); j++) {
		if (s[j] == 'x' || s[j] == 'X') {
			int k = 0;
			for (k = j + 1; s[k] != ','&&k < s.length(); k++) {
				rd_uj += s[k];
			}
			for (int m = k; m < s.length(); m++) {
				if (s[m] != ' '&&s[m] != ',')offset_uj += s[m];
			}
			break;
		}
	}
	rd_uj = fix(binary(rd_uj), 5);
	offset_uj = find_address(offset_uj);
	if (offset_uj[0] == '-') {
		stringstream chuj;
		int chiuj;
		chuj << offset_uj;
		chuj >> chiuj;
		chiuj += 524288;
		stringstream reuj;
		reuj << chiuj;
		reuj >> offset_uj;
	}
	offset_uj = fix(binary(offset_uj), 19) + "0";
	//reset correct seat of offset
	string temp_offset;
	temp_offset = offset_uj;
	offset_uj = temp_offset[0];
	for (int j = 10; j < 20; j++) {
		offset_uj += temp_offset[j];
	}
	offset_uj += temp_offset[9];
	for (int j = 1; j < 9; j++) {
		offset_uj += temp_offset[j];
	}

	//output answer of UJ-type
	outfile << offset_uj << rd_uj << opcode_uj << endl;
	//reset
	opcode_uj = "";
	rd_uj = "";
	offset_uj = "";
}



int main() {
	while (getline(infile, inst_s[i_num]) && inst_s[i_num] != "") {
		check_label(inst_s[i_num]);
		if (inst_s[i_num] != "") {
			cut(inst_s[i_num]);
		}
		i_num++;
	}
	/*
	for (int i = 0; i < i_num; i++) {
		outfile << inst_s[i] << endl;
		outfile << act[i_num] << endl;
	}
	outfile << endl;
	for (int i = 0; i < lebal_num; i++) {
		outfile << lebal_name[i] << "  " << lebal_loc[i] << endl;
	}
	*/
	//================================================================
	for (i_current = 0; i_current < i_num; i_current++) {
		//outfile << i_current << endl;
		if (inst_s[i_current] == "")outfile << endl;
		else {
			//-----------R-type------------------------------------------------
			if (act[i_current] == "add") {
				func3_r = "000";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "sub") {
				func3_r = "000";
				func7_r = "0100000";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "sll") {
				func3_r = "001";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "slt") {
				func3_r = "010";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "sltu") {
				func3_r = "011";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "xor") {
				func3_r = "100";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "srl") {
				func3_r = "101";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "sra") {
				func3_r = "010";
				func7_r = "0100000";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "or") {
				func3_r = "110";
				getvalue_r(inst_s[i_current]);
			}
			else if (act[i_current] == "and") {
				func3_r = "111";
				getvalue_r(inst_s[i_current]);
			}

			//----------S-type------------------------------
			else if (act[i_current] == "sb") {
				func3_s = "000";
				getvalue_s(inst_s[i_current]);
			}
			else if (act[i_current] == "sh") {
				func3_s = "001";
				getvalue_s(inst_s[i_current]);
			}
			else if (act[i_current] == "sw") {
				func3_s = "010";
				getvalue_s(inst_s[i_current]);
			}
			else if (act[i_current] == "sd") {
				func3_s = "011";
				getvalue_s(inst_s[i_current]);
			}

			//--------I-type(offset)----------------
			else if (act[i_current] == "lb") {
				func3_io = "000";
				getvalue_io(inst_s[i_current]);
			}
			else if (act[i_current] == "lh") {
				func3_io = "001";
				getvalue_io(inst_s[i_current]);
			}
			else if (act[i_current] == "lw") {
				func3_io = "010";
				getvalue_io(inst_s[i_current]);
			}
			else if (act[i_current] == "ld") {
				func3_io = "011";
				getvalue_io(inst_s[i_current]);
			}
			else if (act[i_current] == "lbu") {
				func3_io = "100";
				getvalue_io(inst_s[i_current]);
			}
			else if (act[i_current] == "lhu") {
				func3_io = "101";
				getvalue_io(inst_s[i_current]);
			}
			//--------I-type(immediate)----------------
			else if (act[i_current] == "addi") {
				func3_ii = "000";
				getvalue_ii(inst_s[i_current]);
			}
			else if (act[i_current] == "slti") {
				func3_ii = "001";
				getvalue_ii(inst_s[i_current]);
			}
			else if (act[i_current] == "sltiu") {
				func3_ii = "010";
				getvalue_ii(inst_s[i_current]);
			}
			else if (act[i_current] == "xori") {
				func3_ii = "011";
				getvalue_ii(inst_s[i_current]);
			}
			else if (act[i_current] == "ori") {
				func3_ii = "100";
				getvalue_ii(inst_s[i_current]);
			}
			else if (act[i_current] == "andi") {
				func3_ii = "101";
				getvalue_ii(inst_s[i_current]);
			}
			/*   jalr rd,rs1,offset    */
			else if (act[i_current] == "jalr") {
				func3_ii = "000";
				opcode_ii = "1100111";
				getvalue_ii(inst_s[i_current]);
			}
			//---------R-type(rv64i)------------------
			else if (act[i_current] == "slli") {
				func3_rup = "001";
				getvalue_rup(inst_s[i_current]);
			}
			else if (act[i_current] == "srli") {
				func3_rup = "101";
				getvalue_rup(inst_s[i_current]);
			}
			else if (act[i_current] == "srai") {
				func3_rup = "101";
				func6_rup = "010000";
				getvalue_rup(inst_s[i_current]);
			}
			//---------SB-type-----------------------
			else if (act[i_current] == "beq") {
				func3_sb = "000";
				getvalue_sb(inst_s[i_current]);
			}
			else if (act[i_current] == "bne") {
				func3_sb = "001";
				getvalue_sb(inst_s[i_current]);
			}
			else if (act[i_current] == "blt") {
				func3_sb = "100";
				getvalue_sb(inst_s[i_current]);
			}
			else if (act[i_current] == "bge") {
				func3_sb = "101";
				getvalue_sb(inst_s[i_current]);
			}
			else if (act[i_current] == "bltu") {
				func3_sb = "110";
				getvalue_sb(inst_s[i_current]);
			}
			else if (act[i_current] == "bgeu") {
				func3_sb = "111";
				getvalue_sb(inst_s[i_current]);
			}
			//---------U-type-----------------------
			else if (act[i_current] == "lui") {
				opcode_u = "0110111";
				getvalue_u(inst_s[i_current]);
			}
			else if (act[i_current] == "auipc") {
				opcode_u = "0010111";
				getvalue_u(inst_s[i_current]);
			}
			//---------UJ-type-----------------------
			else if (act[i_current] == "jal") {
				opcode_uj = "1101111";
				getvalue_uj(inst_s[i_current]);
			}
		}

	}

	infile.close();
	outfile.close();
}