
namespace ml {

	//! returns the time in seconds
	double Timer::getTime()	{
#ifdef _WIN32
		unsigned __int64 pf;
		QueryPerformanceFrequency( (LARGE_INTEGER *)&pf );
		double freq_ = 1.0 / (double)pf;

		unsigned __int64 val;
		QueryPerformanceCounter( (LARGE_INTEGER *)&val );
		return (val) * freq_;
#endif //_WIN32

#ifdef __linux__
		struct timeval timevalue;
		gettimeofday(&timevalue, nullptr);
		return (double)((UINT64)timevalue.tv_sec) + (double)((UINT64)timevalue.tv_usec) / 1000000.0;
#endif //LINUX
	}
} // namespace ml