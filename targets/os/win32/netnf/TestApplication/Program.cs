using System.Threading;

namespace NF.TestApplication_NEW
{
    public class Program
    {

        public static void Main()
        {

            int thisIsAnInteger = 64;

            var testString = thisIsAnInteger.ToString();

            while (true)
            {
                Thread.Sleep(1000);
            }
        }
    }
}
