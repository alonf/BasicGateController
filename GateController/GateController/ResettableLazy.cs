using System;
using System.Threading;

namespace GateController
{
    public class ResettableLazy<T>
    {
        private Lazy<T> _container;
        
        private readonly Func<Lazy<T>> _resetFunction;

        public ResettableLazy(Func<Lazy<T>> resetFunction = null)
        {
            _resetFunction = resetFunction ?? (() => new Lazy<T>());
            _container = _resetFunction();
        }

        public ResettableLazy(bool isThreadSafe) : this(() => new Lazy<T>(isThreadSafe))
        {
        }
        

        public ResettableLazy(Func<T> valueFactory) : this (() => new Lazy<T>(valueFactory))
        { 
        }

        public ResettableLazy(Func<T> valueFactory, bool isThreadSafe) : this(() =>
            new Lazy<T>(valueFactory, isThreadSafe))
        {

        }

        public ResettableLazy(Func<T> valueFactory, LazyThreadSafetyMode mode) : this(() => new Lazy<T>(valueFactory, mode))
        { 
        }

        public ResettableLazy(LazyThreadSafetyMode mode) : this (() => new Lazy<T>(mode))
        { 
        }


    public T Value => this._container.Value;
        public bool IsValueCreated => this._container.IsValueCreated;

        public void Reset()
        {
            _container = _resetFunction();
        }

    }
}