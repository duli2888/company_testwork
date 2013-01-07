
public class HelloWorld {
	static {
		System.loadLibrary("helloworld");
	}

	native static public void print_who();
	
	static public void main(String args[]) {
	
		print_who();
	}
}
