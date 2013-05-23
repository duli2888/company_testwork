#include <stdio.h>

class A {
	private:
		static int va;
	public:
		static void fa();

};

class B {
	private:
		int vb;
	public:
		void fb();

};

int A::va;
void A::fa()
{
	//A::va = 10;  /* 注：只能放在静态成员变量当中使用,如果放在main中，则会出现错误，因为是private的。 */
	printf("A:fa, A::va = %d\n", A::va);
}


void B::fb()
{
	// vb = 20;
	printf("B:fb\n");
}

int main()
{
	A::fa();
//	B::fb();
}
