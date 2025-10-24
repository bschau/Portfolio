package dk.schau.CitiZr.InstructionsActivity;

import java.util.ArrayList;
import java.util.List;

import dk.schau.CitiZr.CitiZrActivity.CitiZrActivity;
import dk.schau.CitiZr.CredentialsActivity.CredentialsActivity;
import dk.schau.CitiZr.R;
import dk.schau.CitiZr.ShopActivity.ShopActivity;
import dk.schau.CitiZr.Dialog.AboutDialog;

import android.app.Dialog;
import android.app.ListActivity;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

public class InstructionsActivity extends ListActivity {
	private List<Coffee> coffees;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.instructions);        
        
        initCoffees();
        
        CoffeeAdapter ca = new CoffeeAdapter(this, R.layout.instructionitem, coffees);
        setListAdapter(ca);
    }

    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {
    	showInstructionDialog(coffees.get(position));
    }
    
	@Override
 	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.instructionsmenu, menu);
		return true;
 	}
	
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.shop:
				startActivity(new Intent(this, ShopActivity.class));
				return true;
				
			case R.id.list:
				startActivity(new Intent(this, CitiZrActivity.class));
				return true;
	
			case R.id.addlogin:
	        	startActivity(new Intent(this, CredentialsActivity.class));
				return true;
				
			case R.id.about:
				try {
					AboutDialog.show(this);
				} catch (NameNotFoundException e) {
					e.printStackTrace();
				}
				return true;		
		}

		return false;
	}
	
	private void initCoffees() {
		coffees = new ArrayList<Coffee>();
		
		coffees.add(new Coffee(getString(R.string.instructions_caffelatte_name), R.drawable.caffelatte, R.drawable.pisker2, getString(R.string.instructions_caffelatte_body), getString(R.string.instructions_caffelatte_mixing), getString(R.string.instructions_caffelatte_capsule)));
		coffees.add(new Coffee(getString(R.string.instructions_cappuccino_name), R.drawable.cappuccino, R.drawable.pisker1, getString(R.string.instructions_cappuccino_body), getString(R.string.instructions_cappuccino_mixing), getString(R.string.instructions_cappuccino_capsule)));
		coffees.add(new Coffee(getString(R.string.instructions_icedcappuccino_name), R.drawable.icedcappuccino, R.drawable.pisker1, getString(R.string.instructions_icedcappuccino_body), getString(R.string.instructions_icedcappuccino_mixing), getString(R.string.instructions_icedcappuccino_capsule)));
		coffees.add(new Coffee(getString(R.string.instructions_lattemacchiato_name), R.drawable.lattemachiato, R.drawable.pisker1, getString(R.string.instructions_lattemacchiato_body), getString(R.string.instructions_lattemacchiato_mixing), getString(R.string.instructions_lattemacchiato_capsule)));
	}

    private void showInstructionDialog(Coffee coffee) {
        final Dialog dialog = new Dialog(this);
        dialog.setContentView(R.layout.instructiondialog);
        dialog.setTitle(coffee.getName());
        dialog.setCancelable(true);
        dialog.setCanceledOnTouchOutside(true);

		((ImageView) dialog.findViewById(R.id.image)).setImageResource(coffee.getImageId());
		((ImageView) dialog.findViewById(R.id.whip)).setImageResource(coffee.getWhipId());
		((TextView) dialog.findViewById(R.id.name)).setText(coffee.getName());
		((TextView) dialog.findViewById(R.id.recommendation)).setText(getString(R.string.instructions_recommended) + " " + coffee.getRecommendations());
		((TextView) dialog.findViewById(R.id.scale)).setText(coffee.getScale());
		((TextView) dialog.findViewById(R.id.description)).setText(coffee.getDescription());

		Button button = (Button) dialog.findViewById(R.id.ok);
        button.setOnClickListener(new OnClickListener() {
        	public void onClick(View v) {
        		dialog.cancel();
            }
        });
        
        dialog.show();
    }
}
