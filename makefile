compile: 
	make libAPI
	make libcompilejob
	make libparsejob
	make liboutputjob
	make libllmjob
	make libfsgenjob
	make libcustomjob
	make build

libAPI:
	clang++ -shared -o ./Data/libAPI.so -std=c++14 ./Code/lib_job_system/jobsystemapi.cpp ./Code/lib_job_system/jobsystem.cpp ./Code/lib_job_system/workerthread.cpp -fPIC -lpthread -ldl -I$(xcrun --show-sdk-path)/usr/include/c++/v1

libcompilejob:
	clang++ -shared -o ./Data/libFetchCompileJob.so -std=c++14 ./Code/lib_job_system/compilejob.cpp ./Code/lib_job_system/parsejob.cpp ./Code/lib_job_system/outputjob.cpp -fPIC -L./Data -lAPI -I$(xcrun --show-sdk-path)/usr/include/c++/v1

libparsejob:
	clang++ -shared -o ./Data/libFetchParseJob.so -std=c++14 ./Code/lib_job_system/parsejob.cpp ./Code/lib_job_system/outputjob.cpp -fPIC -L./Data -lAPI -I$(xcrun --show-sdk-path)/usr/include/c++/v1

liboutputjob:
	clang++ -shared -o ./Data/libFetchOutputJob.so -std=c++14 ./Code/lib_job_system/outputjob.cpp -fPIC -I$(xcrun --show-sdk-path)/usr/include/c++/v1

libllmjob:
	clang++ -shared -o ./Data/libFetchLLMJob.so -std=c++14 ./Code/lib_job_system/llmjob.cpp -fPIC -I$(xcrun --show-sdk-path)/usr/include/c++/v1

libfsgenjob:
    clang++ -shared -o ./Data/libFetchFSGenJob.so -std=c++14 ./Code/lib_job_system/fsgenjob.cpp -fPIC -I$(xcrun --show-sdk-path)/usr/include/c++/v1

libcustomjob:
	clang++ -shared -o ./Data/libFetchCustomJob.so -std=c++14 ./Code/lib_job_system/customjob.cpp -fPIC -I$(xcrun --show-sdk-path)/usr/include/c++/v1

build:
	clang++ -g -std=c++14 -o app ./Code/main.cpp ./Code/fsparser.cpp -L./Data -lAPI -Wl,-rpath,./Data -I$(xcrun --show-sdk-path)/usr/include/c++/v1

run:
	./app

automated:
	clang++ -g -std=c++14 ./Code/lib_job_system/samplecode/*.cpp -o automated
	./automated

