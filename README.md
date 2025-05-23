# Random Key Generator
In recent years it's very common for people to use shitty passwords or they wind up using the 
same password everywhere. It can be a pain to have to pay money for a password manager let alone 
renting a vps for one. This tool is the Poor-Mans-Option when it comes to doing stuff like that. 
This tool was my solution to solving the problem of support and requirements to fit everybody's 
needs so no need to download python, node-js or etc.

Know that smarter solutions exist and that this should be a backup if you can't get a 
password-manager.


## Installation Requirements

## Windows & Apple Devices
- Going to the releases should do the trick 

## Linux
in order for clipboard features to work properly, the rust clippers crate asks for you to downlod  
`libx11-dev/libX11-devel` and `libpng-dev/libpng-devel` 



This tool can help you come up with a password or Generating a Large key for important logins
or it can be utilized with a few different tools such as 
- 7zip
- Cryptomater (Best Practiced with wormhole.app or Google Drive or Microsoft Onedrive)
- Veracrypt 
- Your Account Logins


## How to use 
If you need to generate a file do this 

```
rkg -s 25 -o <your-file>.key (txt will work but just know that others can gain access to txts a bit more easily.)
```

If you would like to copy the output do this

```
rkg --clipboard
```

You can also specify what your key's ascii characters should be , in this example numbers `0-9` will not be utilized 
to fit another tool's requirements.

```
rkg --lowercase --uppercase
```



# Important Advice How To Best Utilize these tools with Different Software Encryption Tools

## Veracrypt Usage
- If your planning to use __Veracrypt__ it might not be a smart idea to generate a key 
    into the same computer as the encrypted volume. Someone trying to bust into your files open will be able to acess it pretty swiftly and open your files.

    - Storing on a __USB__ Would be a smarter choice but there's a better option shown below

    - Your best case scenario would be to write the key it generates onto a peice of paper and store it in a safe.




## USB Tips

- If Malware has infected your device __Do not plug in your USB Drive to your infected device__ if that was where you stored your important keys Malware can be programed as a USB-Worm and dig up your important passwords and keys!



## Cryptomater or 7zip Or Anything you plan to upload to a Server


- __DO NOT TO PUT YOUR KEYS ON THE CLOUD, THIS SHOULD BE COMMON SENSE FOR ANYBODY READING THIS!!!__  If someboy sees it they will be able to quickly decrypt your important files!!!
    
- If you planning on sharing the files with someone else a DM over Discord / Signal / Element, etc with your key file 
    should do the trick, remeber to be aware of who you send your key to and that the key you used is not the same as your real account password.

## Last But Not Least!!!

- This Software Can't Protect you if
    - You Stored your key in a not safe hiding spot.
    - Malware gets to your key
    - If You loose your key and the software can't recover it. 
    - If you left your important files decrypted.
    - You did any of these wrong
    - The Person your sharing keys with is untrustworthy.
    




