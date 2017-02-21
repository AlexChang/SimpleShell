/**
 * Created by alex on 16-11-4.
 */

import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;

public class SimpleShell {
    public static void main(String[] args)throws IOException{
        String commandLine;
        BufferedReader console=new BufferedReader(new InputStreamReader(System.in));
        while(true){
            System.out.print("jsh>");
            commandLine=console.readLine();
            if(commandLine==null) System.exit(0);
            if(commandLine.trim().equals("")) continue;
            String[] tokens=commandLine.trim().split("\\s+");
            ArrayList<String> commands=new ArrayList<String>(Arrays.asList(tokens));
            OSProcess osp=new OSProcess(commands);
            osp.process();
        }
    }
}

