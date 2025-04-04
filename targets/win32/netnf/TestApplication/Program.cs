using System;
using System.Diagnostics;
using System.Threading;

namespace NF.TestApplication_NEW
{
    public class Program
    {
        public interface TInt
        {
            void Do1();
            void Do2();
        }

        public static void StaticGenericDo<T1, T2>(T1 val, T2 val2) where T1:TInt where T2 : TInt
        {
            Debug.WriteLine("Doing things");
            val.Do1();
            val.Do2();
            val2.Do1();
            val2.Do2();
        }

        public class IntDo : TInt
        {
            public void Do1()
            {
                Debug.WriteLine("Int::Do1");
            }

            public void Do2()
            {
                Debug.WriteLine("Int::Do2");
            }
        }

        public class StringDo : TInt
        {
            public void Do1()
            {
                Debug.WriteLine("String::Do1");
            }

            public void Do2()
            {
                Debug.WriteLine("String::Do1");
            }
        }

        static Task B()
        {
            Debug.WriteLine("B");
            return new Task(() =>
            {
                Debug.WriteLine("B:Task");
                for (int i = 0; i < 10000; i++)
                {
                }
                Debug.WriteLine("B:Done");
            });
        }

        static Task<int> C()
        {
            Debug.WriteLine("C");
            var task = new Task<int>(() =>
            {
                Debug.WriteLine("C:Long Task");
                for (int i = 0; i < 1000000; i++)
                {
                }
                Debug.WriteLine("C:Done");
                return 1;
            });
            //Debug.WriteLine("C:End");
            //var task = Task.FromResult(1);
            Debug.WriteLine("C:Ret");
            return task;
        }

        static async void AsyncTest()
        {
            //await B();
            //await B();
            Debug.WriteLine("XXX");
            var t = C();
            Debug.WriteLine("Got C");
            var c = await t;
            Debug.WriteLine($"Result {c}");
        }


        class GenericClass<T>
        {
            public void InstanceGenericDo(T t)
            {
                T t2 = t;
                Debug.WriteLine($"TClassDo {t2}");
            }

            public void InstanceGenericDo2<T2>(T t, T2 t2)
            {
                T _t = t;
                Debug.WriteLine($"TClassDo {_t} {t2}");
            }
        }

        public static void Main()
        {
            Debug.WriteLine("Hello");
            var m = new GenericClass<int>();
            m.InstanceGenericDo(1);
            m.InstanceGenericDo2(1, "OK");
            var m2 = new GenericClass<string>();
            m2.InstanceGenericDo("OK");
            m2.InstanceGenericDo2("OK", "Now");
            StaticGenericDo(new IntDo(), new StringDo());
            StaticGenericDo(new StringDo(), new IntDo());
            Debug.WriteLine("World");
            AsyncTest();
            while (true)
            {
                Thread.Sleep(1000);
            }
        }
    }
}
