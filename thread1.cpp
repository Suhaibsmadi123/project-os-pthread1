#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

mutex stt;
mutex Ab;
mutex Eq;
mutex Be;
mutex co;

int state = 0;
int above = 0;
int equals = 0;
int below = 0;

void merge(vector<int>& array, int left, int middle, int right, int th) {
    int left_slice_size = middle - left + 1;
    vector<int> left_slice = vector<int>(left_slice_size);
    for (int i = 0; i < left_slice_size; i++) {
        left_slice[i] = array[left + i];
    }

    int right_slice_size = right - middle;
    vector<int> right_slice = vector<int>(right_slice_size);
    for (int j = 0; j < right_slice_size; j++) {
        right_slice[j] = array[middle + j + 1];
    }

    int k = left, i = 0, j = 0;
    for (k = left; i < left_slice_size && j < right_slice_size; k++) {
        if (left_slice[i] <= right_slice[j]) {
            array[k] = left_slice[i++];
        }
        else {
            array[k] = right_slice[j++];
        }
    }

    while (i < left_slice_size) {
        array[k++] = left_slice[i++];
    }

    while (j < right_slice_size) {
        array[k++] = right_slice[j++];
    }
}

void mergeSort(vector<int>& arr, int left, int right, int th, int thnum) {
    if (left >= right) {
        return;
    }

    int middle = left + (right - left) / 2;

    // Sort first and second halves
    mergeSort(arr, left, middle, th, thnum);
    mergeSort(arr, middle + 1, right, th, thnum);

    // Merge the sorted halves
    merge(arr, left, middle, right, th);

    int min = arr[0];
    int max = arr[arr.size() - 1];

    if (thnum != -1) {
        cout << "Thread " << thnum << ": max= " << max << ", min= " << min << endl;
    }
}

void thread_sort(std::vector<int>& arr, int th, int thnum) {
    cout << "Starting thread " << thnum << ": low=" << arr[0]
        << ", high=" << arr[arr.size() - 1] << endl;

    int left = 0;
    int right = arr.size() - 1;
    mergeSort(arr, left, right, th, thnum);

    for (int j = 0; j < 1; j++) {
        for (int i = left; i <= right; i++) {
            if (arr[i] < th) {
                // Be.lock();
                below++;
                // Be.unlock();
            }
            else if (arr[i] > th) {
                // Ab.lock();
                above++;
                // Ab.unlock();
            }
            else {
                // Eq.lock();
                equals++;
                // Eq.unlock();
            }
        }
    }
}

void thread_nop(int i) {
    cout << "Thread number" << i << " they have no work to do" << endl << endl;
}

int main() {
    ifstream input = ifstream("in.txt");
    if (!input.is_open()) {
        cerr << "Unable to open input file." << endl;
        return EXIT_FAILURE;
    }

    int L, TH;
    input >> L >> TH;

    vector<int> List;
    int count = 0;
    for (int i = 0; i < L; i++) {
        int c;
        input >> c;
        List.emplace_back(c);
    }
    input.close();

    std::vector<std::thread> threads;

    std::vector<int> sorted_values;
    std::vector<std::vector<int>> arrays;
    if (List.empty()) {
        cout << "List is empty" << endl;
    }

    int T;
    cout << "Enter number of Thread: ";
    cin >> T;

    count = 0;
    for (int i = 0; i < T; i++) {
        vector<int> tem;
        for (int j = 0; j < L / T; j++) {
            tem.emplace_back(List[count++]);
        }
        arrays.push_back(tem);
    }

    // int i = 0;
    // for (auto array : arrays) {
    //   cout << "Thread " << i++ << ": ";
    //   for (auto num : array) {
    //     cout << num << " ";
    //   }
    //   cout << endl;
    // }

    cout << "Main: Starting sorting with N=" << L << ", TH=" << TH << endl;
    for (int thnum = 0; thnum < T; thnum++) {
        // if (thnum < L) {
        threads.push_back(thread(thread_sort, ref(arrays[thnum]), TH, thnum));
        // } else {
        //   threads.push_back(std::thread(f, thnum));
        // }
    }

    for (int i = 0; i < T; ++i) {
        threads[i].join();
    }

    for (int i = 0; i < arrays.size(); ++i) {
        sorted_values.insert(sorted_values.end(), arrays[i].begin(),
            arrays[i].end());
        mergeSort(sorted_values, 0, sorted_values.size() - 1, TH, -1);
    }

    cout << "Main: Above Threshold = " << above << endl;
    cout << "Main: Equals Threshold = " << equals << endl;
    cout << "Main: Below Threshold = " << below << endl;

    ofstream output = ofstream("out.txt");
    if (!output.is_open()) {
        cerr << "Unable to open output file." << endl;
        return EXIT_FAILURE;
    }

    output << "Sorted array:" << endl;
    for (int val : sorted_values) {
        output << val << " ";
    }
    output.close();

    return 0;
}
