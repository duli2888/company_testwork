
public class HelloWorld {
	static {
		System.loadLibrary("dladd");
	}

	native static public int add(int a, int b);
	
	static public void main(String args[]) {
		int a = 1;
		int b = 2;
		int sum = 0;
	
		sum = add(a, b);

		System.out.println(sum);
	}
}
