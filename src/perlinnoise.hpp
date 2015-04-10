#ifndef PERLINNOISE_H
#define PERLINNOISE_H

class PerlinNoise {
public:
	static double GetHeight(int x, int z, double max, double scale) {
		return noise(x * scale, z * scale) * max;
	}
	static double findNoise(double x,double y) {
		int n=(int)x+(int)y*57;
		n=(n<<13)^n;
		int nn=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
		return 1.0-((double)nn/1073741824.0);
	}
	static double interpolate(double a,double b,double x)
	{
		double ft=x * 3.1415927;
		double f=(1.0-cos(ft))* 0.5;
		return a*(1.0-f)+b*f;
	}
	static double noise(double x,double y)
	{
		double floorx=(double)((int)x);
		double floory=(double)((int)y);
		double s,t,u,v;
		s=findNoise(floorx,floory); 
		t=findNoise(floorx+1,floory);
		u=findNoise(floorx,floory+1);
		v=findNoise(floorx+1,floory+1);
		double int1=interpolate(s,t,x-floorx);
		double int2=interpolate(u,v,x-floorx);
		return interpolate(int1, int2, y - floory);
	}

private:
};

#endif