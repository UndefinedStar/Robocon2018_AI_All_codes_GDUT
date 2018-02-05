#define g 9.826
#include<iostream>
#include<math.h>

using namespace std;

int main()
{
	float H,m,h,y;//已知量
	float a, b, c, k, V, angle;//未知量
	float v[2], A[2];
	int flat = 0;
	cout << "抛圆环选1，抛金环选2" << endl;
	cin >> flat;
	if (flat == 1)
	{
		cout << "输入圆环的高，2~2.8" << endl;
		cin >> H;
		cout << "输入抛出点的高h" << endl;
		cin >> h;
		cout << "抛出点所在的世界坐标系的y坐标" << endl;
		cin >> y;
		m = 7.035;
		float tmp = (y*y - 2 * m*y + m*m);
		a = (h - H) / tmp;
		b = -2 * m*a;
		c = H + m*m*a;
		k = (2 * (y - m)*(h - H)) / tmp;
		float sincos = k / (k*k + 1);
		V = sqrt(g*(m - y) / sincos);
		angle = atan(k);
		cout << "如果要通过圆环，抛射初速度为 " << V << " " << "仰角为 " << angle*180 << endl;
	}
	if (flat == 2)
	{
		cout << "输入圆环的高，3~3.8" << endl;
		cin >> H;
		cout << "输入抛出点的高h" << endl;
		cin >> h;
		cout << "抛出点所在的世界坐标系的y坐标" << endl;
		cin >> y;
		m = 7.035;
		float xx = 0;
		for (int i = 0;i < 1;i++)
		{
			if (i = 0)
				xx = 10.135;
			else
				xx = 11.335;
			float tmp = ((0.4 - h) / (i - y) - (H - h) / (m - y));
			a = tmp / (i - m);
			b = (H - h) / (m - y) - (m + y)*a;
			c = H - (H - h)*m / (m - y) + m*y*a;
			k = tmp*(y - m) / (i - m) + (H - h) / (m - y);
			v[i] = sqrt(g*(m - y)*(m - y)*(k*k + 1) / (2 * (k*(m - y) + h - H)));
			A[i] = atan(k);
		}
		cout << "如果要通过圆环，抛射初速度为 " << v[0] << " ~ " << v[1] << " " << "仰角为 " << A[0] << " ~ " << A[1] << endl;
	}
	system("pause");
	return 0;
}