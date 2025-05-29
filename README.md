# spider

socks5 proxy tunnel tool

> [!NOTE]
> This tool was created for learning purposes.
>
> Therefore, I recommend using other tunnel tools. (e.g. ligolo-ng, sshuttle, chisel, ...)

## How it works
![](./imgs/img01.jpg)

The spider has the following features:

- Can have multiple spider nodes (client, server, pipe)
- Connects to multiple spider nodes
- Generates and exchanges routing information between spiders
- Automatically generates a spider server node at the desired spider
- Automatically forwards SOCKS5 packets to the desired spider
- Forwards packets from the SOCKS5 client from the spider client node to the desired spider server node
- Establishes a connection to the destination server from the spider server node
- Delete the spider server node automatically after the SOCKS5 communication ends

## License
This project is licensed under the MIT License.

See the [LICENSE](https://github.com/shuichiro-endo/spider/blob/main/LICENSE) file for details.
