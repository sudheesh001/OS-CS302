/**Implementation of BOXPLOT (a data-visualization technique)**/

import java.awt.Color;
import java.awt.Graphics;
import java.util.Scanner;
import javax.swing.JFrame;

public class Boxplot extends JFrame implements Runnable {

	private static final long serialVersionUID = 1L;
	public static int[][] items;
	static int num_items[];
	static int num_sets;
	static int max_element, min_element, division, Max_Height = 750,
			Max_Width = 1360, Base_Hori = 40, Base_Vert = 700,
			no_of_divisions = 20;
	static float scale;
	private static Scanner scan;

	public static void main(String args[]) {
		scan = new Scanner(System.in);
		System.out.println("Enter the number of sets\n");
		int i = 0;
		int j;

		num_sets = scan.nextInt();
		items = new int[num_sets][1000];
		num_items = new int[num_sets];
		for (i = 0; i < num_sets; i++) {
			System.out.println("Enter the number of items in  set:" + (i + 1));
			num_items[i] = scan.nextInt();

			System.out.println("Enter the items in set:" + (i + 1));

			for (j = 0; j < num_items[i]; j++) {
				items[i][j] = scan.nextInt();
			}
		}
		int k;
		for (k = 0; k < num_sets; k++) {// sort the kth set of elements..
			for (i = 0; i < num_items[k]; i++) {
				for (j = 0; j < num_items[k] - 1; j++) {
					if (items[k][j] > items[k][j + 1]) {
						int temp = items[k][j];
						items[k][j] = items[k][j + 1];
						items[k][j + 1] = temp;
					}
				}
			}
		}

		for (k = 0; k < num_sets; k++) {// sort the kth set of elements..
			for (i = 0; i < num_items[k]; i++) {
				System.out.println(items[k][i] + " ");
			}
			System.out.println();
		}
		max_element = items[0][0];
		min_element = items[0][0];
		for (i = 0; i < num_sets; i++) {
			for (j = 0; j < num_items[i]; j++) {
				if (items[i][j] > max_element) {
					max_element = items[i][j];
				}
				if (items[i][j] < min_element) {
					min_element = items[i][j];
				}
			}
		}
		
		division = Base_Vert / no_of_divisions;
		scale = ((max_element + 500) * division) / Base_Vert;//i.e. one division on y-axis represents "scale" units of element..
		System.out.println("Scale: " + scale);
		Thread t = new Thread();
		t.start();
		Boxplot b = new Boxplot();

		b.setSize(Max_Width, Max_Height);
		b.setVisible(true);
		b.repaint();

	}

	Boxplot() {

	}

	public void paint(Graphics g) {
		
		g.drawLine(Base_Hori - 0, Base_Vert - 0, 1300, Base_Vert - 0);
		g.drawLine(Base_Hori - 0, 0, Base_Hori - 0, Base_Vert);
		int i, X_coor = 50, width = 40;

		g.setColor(Color.CYAN);
		for (i = 1; i < no_of_divisions; i++) {
			g.drawLine(5, Base_Vert - (i * division), 1300, Base_Vert -  (i * division));
		}
		g.setColor(Color.BLACK);
		for (i = 0; i < num_sets; i++) {
			g.setColor(Color.BLACK);

			int median = items[i][num_items[i] / 2];
			int q1 = items[i][num_items[i] / 4];
			int q3 = items[i][(3 * num_items[i]) / 4];
			g.drawLine(Base_Hori + X_coor,Base_Vert - (int) (((float)q3 / scale) * division),Base_Hori + (X_coor + width),Base_Vert - (int) (((float)q3 / scale) * division));//
			g.drawLine(Base_Hori + X_coor, Base_Vert - (int) (((float)q1 / scale) * division), Base_Hori + (X_coor + width), Base_Vert - (int) (((float)q1 / scale) * division));//(int) (((float)(q3-q1) / scale) * division));
			g.drawLine(Base_Hori + X_coor, Base_Vert - (int) (((float)q3 / scale) * division), Base_Hori + X_coor, Base_Vert - (int) (((float)q1 / scale) * division));
			g.drawLine(Base_Hori + (X_coor + width), Base_Vert - (int) (((float)q3 / scale) * division), Base_Hori + (X_coor + width), Base_Vert - (int) (((float)q1 / scale) * division));
			g.drawLine(Base_Hori + X_coor, Base_Vert - (int) (((float)median / scale) * division), Base_Hori + (X_coor + width), Base_Vert - (int) (((float)median / scale) * division));
			
			g.drawString(Integer.toString(q3), Base_Hori + (X_coor + (width/2)), Base_Vert - (int) (((float)q3 / scale) * division) - 3);
			g.drawString(Integer.toString(q1), Base_Hori + (X_coor + (width/2)), Base_Vert - (int) (((float)q1 / scale) * division) - 3);
			g.drawString(Integer.toString(median), Base_Hori + (X_coor + (width/2)), Base_Vert - (int) (((float)median / scale) * division) - 3);
			
			
			int upper_whisker = (int) (((float)items[i][num_items[i] - 1] / scale) * division);
			if(upper_whisker > (int)(1.5*(q3-q1) + q3))
				upper_whisker = (int) (((float)(1.5*(q3-q1) + q3) / scale) * division);
			
			int lower_whisker = (int) (((float)items[i][0] / scale) * division);
			if(lower_whisker < (int)(q1 - 1.5*(q3-q1)))
				lower_whisker = (int)(((float)(q1 - 1.5*(q3-q1)) / scale) * division);
			
			g.setColor(Color.RED);
			
			g.drawLine(Base_Hori + (X_coor + (width/2)), Base_Vert - upper_whisker, Base_Hori + (X_coor + (width/2)), Base_Vert - (int) (((float)q3 / scale) * division));
			g.drawLine(Base_Hori + (X_coor + (width/2)), Base_Vert - lower_whisker, Base_Hori + (X_coor + (width/2)), Base_Vert - (int) (((float)q1 / scale) * division));
			g.drawLine(Base_Hori + (X_coor + (width/4)), Base_Vert - upper_whisker, Base_Hori + (X_coor + (3*width/4)), Base_Vert - upper_whisker);
			g.drawLine(Base_Hori + (X_coor + (width/4)), Base_Vert - lower_whisker, Base_Hori + (X_coor + (3*width/4)), Base_Vert - lower_whisker);
			
			g.drawString(Integer.toString(items[i][num_items[i] - 1]), Base_Hori + (X_coor + (width/2)), Base_Vert - upper_whisker - 3);
			g.drawString(Integer.toString(items[i][0]), Base_Hori + (X_coor + (width/2)), Base_Vert - lower_whisker - 3);
			
			X_coor += 100;
		}
		//g2d.setTransform(at);
		g.setColor(Color.BLACK);
		for(i=0;i<no_of_divisions;i++){
			int index = (int) (i*division);
			g.drawString(Integer.toString((int)(i*scale)), Base_Hori - 20, Base_Vert - index);
		}
	}

	@Override
	public void run() {
		while(true){
			repaint();
		}
		
	}

}
