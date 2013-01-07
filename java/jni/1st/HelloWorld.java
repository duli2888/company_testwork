
public class HelloWorld {
	static {
		System.loadLibrary("hello");
	}

	public native void DisplayHello();

	public static void main(String args[]) {
		new HelloWorld().DisplayHello();
	}

}
