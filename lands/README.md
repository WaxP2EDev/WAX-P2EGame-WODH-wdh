# wodh-smart-contract

The drhemsworlds contract is designed to...

# Dependencies

* eosio 2.0^
* eosio.cdt 1.6^
* cmake 3.5^

# Compiling

```
./build.sh -e /root/eosio/2.0 -c /usr/opt/eosio.cdt
```

# Deploying

```
cleos set contract <your_account> ./build/Release/drhemsworlds drhemsworlds.wasm drhemsworlds.abi
```