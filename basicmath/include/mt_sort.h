#pragma once



namespace basicmath {

	class mt_sort {
	public:

		template<class T>
		static void sort(vector<i32>& indexs, vector<T>& data, b8 increase) {
			indexs.resize(data.size(), 0);

			for (i32 i = 0; i < (i32)data.size(); ++i) {
				indexs[i] = i;
			}

			for (i32 i = 0; i < (i32)indexs.size() - 1; ++i) {
				for (i32 j = 0; j < (i32)indexs.size() - j - 1; ++j) {
					if ((increase && data[j] > data[j + 1])
						|| (!increase && data[j] < data[j + 1])) {
						swap(data[j], data[j + 1]);
						swap(indexs[j], indexs[j + 1]);
					}
				}
			}
		}

		template<class T>
		static void bubble_sort(vector<T>& datas, b8 increase) {
			for (i32 i = 0; i < (i32)datas.size() - 1; ++i) {
				for (i32 j = 0; j < (i32)datas.size() - i - 1; ++j) {
					if (increase) {
						if (datas[j] > datas[j + 1]) {
							swap(datas[j], datas[j + 1]);
						}
					} else {
						if (datas[j] < datas[j + 1]) {
							swap(datas[j], datas[j + 1]);
						}
					}
				}
			}
		}

		template<class T>
		static void heap_sort(vector<T>& datas, b8 increase) {
			b8 max_heap = increase ? sys_true : sys_false;
			init_heap(datas, max_heap);

			for (int i = 0; i < (i32)datas.size() - 1; ++i) {
				i32 end_index = (i32)datas.size() - 1 - i;

				swap(datas[0], datas[end_index]);
				heap_adjust(0, &datas[0], end_index, max_heap);
			}
		}

		template<class T>
		static void init_heap(vector<T>& datas, b8 max_heap) {
			for (i32 i = (i32)datas.size() / 2 - 1; i >= 0; --i) {
				heap_adjust(i, &datas[0], (i32)datas.size(), max_heap);
			}
		}

		template<class T>
		static void heap_adjust(i32 index, T* begin, i32 size, b8 max_heap) {
			i32 left_child_index = index * 2 + 1;

			if (left_child_index >= (i32)size) {
				return;
			}

			i32 right_child_index = left_child_index + 1;

			i32 selected_child_index = left_child_index;

			if (right_child_index < size) {
				if ((max_heap && begin[right_child_index] > begin[selected_child_index])
					|| (!max_heap && begin[right_child_index] < begin[selected_child_index])) {
						selected_child_index = right_child_index;
				}
			}

			if ((max_heap && begin[index] < begin[selected_child_index])
				|| (!max_heap && begin[index] > begin[selected_child_index])) {
					swap(begin[index], begin[selected_child_index]);

					heap_adjust(selected_child_index, begin, size, max_heap);
			}
		}

	protected:

		
	};

}