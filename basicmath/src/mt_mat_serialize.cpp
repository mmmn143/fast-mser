#include "stdafx.h"

namespace basicmath {
	template<class T>
	static void write_1d_mat(sys_strcombine& str, const mt_mat& mat) {
		const u8* ptr_mat_dim0 = mat.data();
		for (int row = 0; row < mat.size()[0]; ++row) {
			const T* ptr_channel = (const T*)ptr_mat_dim0;
			for (int c = 0; c < mat.channel(); ++c) {
				str << ptr_channel[c];

				if (c != mat.channel() - 1) {
					str<<L" ";
				}
			}

			if (row != mat.size()[0] - 1) {
				str<<L", ";
			}

			ptr_mat_dim0 += mat.step()[0];
		}
	}

	template<class T>
	static void write_2d_mat(sys_strcombine& str, const mt_mat& mat) {
		const u8* ptr_mat_dim0 = mat.data();
		for (int row = 0; row < mat.size()[0]; ++row) {
			str << L"\n";

			const u8* ptr_mat_dim1 = ptr_mat_dim0;

			for (int col = 0; col < mat.size()[1]; ++col) {
				const T* ptr_channel = (const T*)ptr_mat_dim1;
				for (int c = 0; c < mat.channel(); ++c) {
					str << ptr_channel[c];

					if (c != mat.channel() - 1) {
						str<<L" ";
					}
				}

				if (col != mat.size()[1] - 1) {
					str<<L", ";
				}

				ptr_mat_dim1 += mat.step()[1];
			}

			ptr_mat_dim0 += mat.step()[0];
		}
	}

	template<class T>
	static void write_3d_mat(sys_strcombine& str, const mt_mat& mat) {
		const u8* ptr_mat_dim0 = mat.data();
		for (int plane = 0; plane < mat.size()[0]; ++plane) {
			str<<L"\n"<< plane << L"-th plane mat:\n";

			const u8* ptr_mat_dim1 = ptr_mat_dim0;

			for (int row = 0; row < mat.size()[1]; ++row) {

				const u8* ptr_mat_dim2 = ptr_mat_dim1;

				for (int col = 0; col < mat.size()[2]; ++col) {
					const T* ptr_channel = (const T*)ptr_mat_dim2;
					for (int c = 0; c < mat.channel(); ++c) {
						str << ptr_channel[c];

						if (c != mat.channel() - 1) {
							str<<L" ";
						}
					}

					if (col != mat.size()[2] - 1) {
						str<<L", ";
					}

					ptr_mat_dim2 += mat.step()[2];
				}

				str << L"\n";
				ptr_mat_dim1 += mat.step()[1];
			}

			ptr_mat_dim0 += mat.step()[0];
		}
	}

	void write(sys_strcombine& str, const mt_mat& mat) {
		wstring depth_channel_str = wstring(L", depth_channel: ") + mt_Depth_Channel::depth_channel_str(mat.depth_channel());

		if (1 == mat.dim()) {
			str<<L"1d mat: "<<mat.size()[0]<<depth_channel_str;
			switch (mat.depth()) {
			case mt_U8:
				write_1d_mat<u8>(str, mat);
				break;
			case mt_I8:
				write_1d_mat<i8>(str, mat);
				break;
			case mt_U16:
				write_1d_mat<u16>(str, mat);
				break;
			case mt_I16:
				write_1d_mat<i16>(str, mat);
				break;
			case mt_U32:
				write_1d_mat<u32>(str, mat);
				break;
			case mt_I32:
				write_1d_mat<i32>(str, mat);
				break;
			case mt_U64:
				write_1d_mat<u64>(str, mat);
				break;
			case mt_I64:
				write_1d_mat<i64>(str, mat);
				break;
			case mt_F32:
				write_1d_mat<float>(str, mat);
				break;
			case mt_F64:
				write_1d_mat<double>(str, mat);
				break;
			}

		} else if (2 == mat.dim()) {
			str<<L"2d mat: "<<mat.size()[0]<<L" * "<<mat.size()[1]<<depth_channel_str;
			switch (mat.depth()) {
			case mt_U8:
				write_2d_mat<u8>(str, mat);
				break;
			case mt_I8:
				write_2d_mat<i8>(str, mat);
				break;
			case mt_U16:
				write_2d_mat<u16>(str, mat);
				break;
			case mt_I16:
				write_2d_mat<i16>(str, mat);
				break;
			case mt_U32:
				write_2d_mat<u32>(str, mat);
				break;
			case mt_I32:
				write_2d_mat<i32>(str, mat);
				break;
			case mt_U64:
				write_2d_mat<u64>(str, mat);
				break;
			case mt_I64:
				write_2d_mat<i64>(str, mat);
				break;
			case mt_F32:
				write_2d_mat<float>(str, mat);
				break;
			case mt_F64:
				write_2d_mat<double>(str, mat);
				break;
			}
		} else if (3 == mat.dim()) {
			str<<L"3d mat: "<<mat.size()[0]<<L" * "<<mat.size()[1]<<L" * "<<mat.size()[2]<<depth_channel_str;
			switch (mat.depth()) {
			case mt_U8:
				write_3d_mat<u8>(str, mat);
				break;
			case mt_I8:
				write_3d_mat<i8>(str, mat);
				break;
			case mt_U16:
				write_3d_mat<u16>(str, mat);
				break;
			case mt_I16:
				write_3d_mat<i16>(str, mat);
				break;
			case mt_U32:
				write_3d_mat<u32>(str, mat);
				break;
			case mt_I32:
				write_3d_mat<i32>(str, mat);
				break;
			case mt_U64:
				write_3d_mat<u64>(str, mat);
				break;
			case mt_I64:
				write_3d_mat<i64>(str, mat);
				break;
			case mt_F32:
				write_3d_mat<float>(str, mat);
				break;
			case mt_F64:
				write_3d_mat<double>(str, mat);
				break;
			}
		}	
	}

	template<class T>
	static void write_data_impl(basicsys::sys_json_writer& writer, const mt_mat& data) {
		mt_array_element_const_iterator iter(data);

		for (;;) {
			const u8* ptr_data = iter.data();

			if (ptr_data == NULL) {
				break;
			}

			const T* real_data = (const T*)ptr_data;

			for (i32 c = 0; c < data.channel(); ++c) {
				writer<<real_data[c];
			}
		}
	}

	void write(basicsys::sys_json_writer& writer, const mt_mat& data) {
		writer<<L"{";

		writer<<L"sizes";

		writer<<L"[";

		for (i32 i = 0; i < data.dim(); ++i) {
			writer<<data.size()[i];
		}

		writer<<L"]";

		writer<<L"depth_channel"<<mt_Depth_Channel::depth_channel_str(data.depth_channel());
		writer<<L"data"<<L"[";

		switch (data.depth()) {
		case mt_I8:
			write_data_impl<i8>(writer, data);
			break;
		case mt_U8:
			write_data_impl<u8>(writer, data);
			break;
		case mt_I16:
			write_data_impl<i16>(writer, data);
			break;
		case mt_U16:
			write_data_impl<u16>(writer, data);
			break;
		case mt_I32:
			write_data_impl<i32>(writer, data);
			break;
		case mt_U32:
			write_data_impl<u32>(writer, data);
			break;
		case mt_I64:
			write_data_impl<i64>(writer, data);
			break;
		case mt_U64:
			write_data_impl<u64>(writer, data);
			break;
		case mt_F32:
			write_data_impl<f32>(writer, data);
			break;
		case mt_F64:
			write_data_impl<f64>(writer, data);
			break;
		}

		writer<<L"]";
		writer<<L"}";
	}

	template<class T>
	static void read_data_impl(u8* mat_data, const basicsys::sys_json_reader& reader) {
		vector<T> buffer;

		reader[L"data"]>>buffer;

		T* ptr_mat_data = (T*)mat_data;

		for (i32 i = 0; i < (i32)buffer.size(); ++i) {
			ptr_mat_data[i] = buffer[i];
		}
	}

	void read(mt_mat& data, const basicsys::sys_json_reader& reader) {
		vector<i32> sizes;
		reader[L"sizes"]>>sizes;

		if (sizes.empty()) {
			data = mt_mat();
		} else {
			mt_Depth_Channel depth_channel = mt_Depth_Channel::depth_channel_i32(reader[L"depth_channel"].to_string());
			data = mt_mat((i32)sizes.size(), &sizes[0], depth_channel);

			switch (data.depth()) {
			case mt_I8:
				read_data_impl<i8>(data.data(), reader);
				break;
			case mt_U8:
				read_data_impl<u8>(data.data(), reader);
				break;
			case mt_I16:
				read_data_impl<i16>(data.data(), reader);
				break;
			case mt_U16:
				read_data_impl<u16>(data.data(), reader);
				break;
			case mt_I32:
				read_data_impl<i32>(data.data(), reader);
				break;
			case mt_U32:
				read_data_impl<u32>(data.data(), reader);
				break;
			case mt_I64:
				read_data_impl<i64>(data.data(), reader);
				break;
			case mt_U64:
				read_data_impl<u64>(data.data(), reader);
				break;
			case mt_F32:
				read_data_impl<f32>(data.data(), reader);
				break;
			case mt_F64:
				read_data_impl<f64>(data.data(), reader);
				break;
			}
		}
	}
}