/**
 * Created by alex on 16-11-4.
 */

import java.io.*;
import java.util.ArrayList;

public class OSProcess {
    private ArrayList<String> commandLine;
    private static ArrayList<ArrayList<String>> commandHistory=new ArrayList<ArrayList<String>>();
    private static File homeDirectory=new File(System.getProperty("user.home"));
    private static File currentDirectory=new File("/");

    public OSProcess(ArrayList<String> tokens){
        commandLine=tokens;
    }

    public void process() throws IOException{
        try{
            if(commandLine.get(0).equals("cd")){
                if(commandLine.size()==1){
                    currentDirectory=homeDirectory;
                }else{
                    if(commandLine.get(1).equals("~")){
                        currentDirectory=homeDirectory;
                    }else if(commandLine.get(1).equals(".")){
                        //System.out.println("You are still here!");
                    }else if(commandLine.get(1).equals("..")){
                        if(currentDirectory.equals(new File("/"))){
                            //System.out.println("You are already at the root directory!");
                        }else{
                            File parentDirectory=new File(currentDirectory.getParent());
                            if(parentDirectory.exists() && parentDirectory.isDirectory()){
                                currentDirectory=parentDirectory;
                            }else{
                                System.out.println("No such directory!");
                            }
                        }
                    }else{
                        File absoluteDirectory=new File(commandLine.get(1));
                        File relativeDirectory=new File(currentDirectory+File.separator+commandLine.get(1));
                        if(absoluteDirectory.exists() && absoluteDirectory.isDirectory()){
                            currentDirectory=absoluteDirectory;
                        }else if(relativeDirectory.exists() && relativeDirectory.isDirectory()){
                            currentDirectory=relativeDirectory;
                        }else{
                            System.out.println("No such directory!");
                        }
                    }
                }
            }else if(commandLine.get(0).equals("history")){
                int count=0;
                for(int i=commandHistory.size()-1; i>=0 && i>=commandHistory.size()-10; --i){
                    System.out.print(count+" ");
                    for(int j=0; j<commandHistory.get(i).size(); ++j){
                        System.out.print(commandHistory.get(i).get(j)+" ");
                    }
                    ++count;
                    System.out.println();
                }
            }else{
                ProcessBuilder pb=null;
                if(commandLine.get(0).equals("!!")){
                    if(commandHistory.size()>=1){
                        pb=new ProcessBuilder(commandHistory.get(commandHistory.size()-1));
                    }else{
                        System.out.println("No history command!");
                        return;
                    }
                }else if(commandLine.get(0).charAt(0)=='!'){
                    int value=Character.getNumericValue(commandLine.get(0).charAt(1));
                    if(commandHistory.size()<value){
                        System.out.println("No such history command!");
                        return;
                    }else{
                        pb=new ProcessBuilder(commandHistory.get(commandHistory.size()-value));
                    }
                }else{
                    pb=new ProcessBuilder(commandLine);
                }
                pb.directory(currentDirectory);
                Process proc=pb.start();
                InputStream is=proc.getInputStream();
                InputStreamReader isr=new InputStreamReader(is);
                BufferedReader br=new BufferedReader(isr);
                String line;
                while((line=br.readLine())!=null) System.out.println(line);
                br.close();
            }
            if(!commandLine.get(0).equals("history") && !commandLine.get(0).startsWith("!")){
                commandHistory.add(commandLine);
            }
        }catch (IndexOutOfBoundsException ioobe){
            System.out.println("Command format error!");
            System.out.println("Please input !! or !x");
            //System.out.println(ioobe);
        }catch (IOException ioe){
            System.out.println(ioe);
        }catch (Exception e){
            System.out.println(e);
        }
    }
}
