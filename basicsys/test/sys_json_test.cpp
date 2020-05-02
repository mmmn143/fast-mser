#include "test.h"

static void write_json_context(sys_json_writer& json_writer) {
	json_writer<<"name"<<"xuhailiang";

	json_writer<<"age"<<"19";

	json_writer<<"citys"<<"[";

	json_writer<<"阜阳";
	json_writer<<"nanjing";
	json_writer<<"beijing";

	json_writer<<"]";

	json_writer<<"friends"<<"[";

	json_writer<<"{";

	json_writer<<"name"<<"chenyidong";
	json_writer<<"age"<<"26";
	json_writer<<"citys"<<"[";

	json_writer<<"luan";
	json_writer<<"nanjing";
	json_writer<<"shenzhen";

	json_writer<<"]";

	json_writer<<"}";

	json_writer<<"{";

	json_writer<<"name"<<"xuwei";
	json_writer<<"age"<<"26";
	json_writer<<"citys"<<"[";

	json_writer<<"hefei";
	json_writer<<"shanghai";

	json_writer<<"]";

	json_writer<<"}";

	json_writer<<"]";

	json_writer<<"empty_map"<<"{"<<"}";

	json_writer<<"b8"<<b8(1);

	vector<i32> numbers;

	numbers.resize(200, 0);

	vector<vector<i32>> numbers2;
	numbers2.push_back(numbers);
	numbers2.push_back(numbers);

	vector<vector<vector<i32>>> numbers3;
	numbers3.push_back(numbers2);
	numbers3.push_back(numbers2);

	json_writer<<"number_array"<<numbers3;

	vector<string> strings;
	strings.push_back("haha");
	strings.push_back("");
	strings.push_back("world");

	vector<vector<string>> strings2;
	strings2.push_back(strings);

	vector<vector<vector<string>>> strings3;
	strings3.push_back(strings2);

	json_writer<<"string_array"<<strings3;
}

static void read_json(sys_json_reader& reader) {
	sys_test_equal(reader["name"].to_string(), "xuhailiang");
	sys_test_equal(reader["age"].to_string(), "19");
	sys_test_equal(reader["citys"][0].to_string(), "阜阳");
	sys_test_equal(reader["citys"][1].to_string(), "nanjing");
	sys_test_equal(reader["citys"][2].to_string(), "beijing");

	sys_test_equal(reader["friends"][0]["name"].to_string(), "chenyidong");
	sys_test_equal(reader["friends"][0]["citys"][0].to_string(), "luan");
	sys_test_equal(reader["friends"][0]["citys"][1].to_string(), "nanjing");
	sys_test_equal(reader["friends"][0]["citys"][2].to_string(), "shenzhen");
	sys_test_equal(reader["friends"][0]["age"].to_string(), "26");

	sys_test_equal(reader["friends"][1]["name"].to_string(), "xuwei");
	sys_test_equal(reader["friends"][1]["citys"][0].to_string(), "hefei");
	sys_test_equal(reader["friends"][1]["citys"][1].to_string(), "shanghai");
	sys_test_equal(reader["friends"][1]["age"].to_string(), "26");

	sys_test_equal(reader["b8"].to_b8(), sys_true);

	i32 size = reader["empty_map"].size();

	vector<vector<vector<i32>>> numbers;
	reader["number_array"]>>numbers;

	basiclog_info2(numbers);

	vector<vector<vector<string>>> strings;
	reader["string_array"]>>strings;

	basiclog_info2(strings);
}

static void test_string_buffer_json() {
	string str;
	sys_string_buffer_writer string_file_writer(&str);
	sys_json_writer json_writer(&string_file_writer);

	write_json_context(json_writer);

	basiclog_info2(str);

	sys_string_buffer_reader string_buffer_reader(str);
	sys_json_reader json_reader(&string_buffer_reader, "");

	read_json(json_reader);
}


static void test_string_file_buffer_json() {
	sys_string_file_buffer_writer string_file_writer("test.txt");
	sys_json_writer json_writer(&string_file_writer);

	write_json_context(json_writer);

	string_file_writer.close();

	sys_string_file_buffer_reader string_file_reader("test.txt");
	sys_json_reader json_reader(&string_file_reader, "");

	read_json(json_reader);
}


static void test_byte_buffer_json() {
	vector<u8> buffer;
	sys_byte_buffer_writer writer(&buffer);
	sys_json_writer json_writer(&writer);

	write_json_context(json_writer);



	//basiclog_info2(str);

	sys_byte_buffer_reader reader(&buffer[0], &buffer[0] + (i32)buffer.size());
	
	i32 size = reader.read_i32();

	basiclog_info2(size);
	basiclog_info2(reader.read_str(size));
	basiclog_info2(reader.read_u8());
	basiclog_info2(reader.read_i32());
	basiclog_info2(reader.read_b8());
	
	sys_json_reader json_reader(&reader, "");

	read_json(json_reader);
}


static void test_byte_file_buffer_json() {
	sys_byte_file_buffer_writer writer("test.btxt");
	sys_json_writer json_writer(&writer);

	write_json_context(json_writer);

	writer.close();

	sys_byte_file_buffer_reader reader("test.btxt");
	sys_json_reader json_reader(&reader, "");

	read_json(json_reader);
}

void sys_json_test::run(vector<string>& argvs) {
	test_string_buffer_json();
	test_string_file_buffer_json();

	test_byte_buffer_json();	
	test_byte_file_buffer_json();
}