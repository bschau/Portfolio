package dk.schau.CitiZr.CitiZrActivity;

import java.util.ArrayList;
import java.util.List;

import dk.schau.CitiZr.CredentialsActivity.CredentialsActivity;
import dk.schau.CitiZr.InstructionsActivity.InstructionsActivity;
import dk.schau.CitiZr.R;
import dk.schau.CitiZr.ShopActivity.ShopActivity;
import dk.schau.CitiZr.Dialog.AboutDialog;

import android.app.Dialog;
import android.app.ListActivity;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.os.Bundle;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class CitiZrActivity extends ListActivity {
	private static final int TASTING_KERN = 4;
	private int[] cups = new int[] { R.drawable.cup1, R.drawable.cup2, R.drawable.cup3, R.drawable.cup4 };
	private String[] capsuleCategory = null;
	private List<Capsule> capsules;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        initCapsules();
        
        CapsuleAdapter ca = new CapsuleAdapter(this, R.layout.listitem, capsules);
        setListAdapter(ca);
    }

    @Override
    protected void onListItemClick(ListView l, View v, int position, long id) {
    	showCapsuleDialog(capsules.get(position));
    }
    
	@Override
 	public boolean onCreateOptionsMenu(Menu menu) {
		super.onCreateOptionsMenu(menu);
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.citizrmenu, menu);
		return true;
 	}
	
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
			case R.id.shop:
				startActivity(new Intent(this, ShopActivity.class));
				return true;
				
			case R.id.instructions:
				startActivity(new Intent(this, InstructionsActivity.class));
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
  
    private void initCapsules() {
        capsules = new ArrayList<Capsule>();
 
        capsules.add(new Capsule(R.drawable.arpeggio, getString(R.string.arpeggio_name),
        		Category.Intenso, R.drawable.str09, new int[] { Cup.Ristretto, Cup.Espresso, Cup.Cappuccino },
                getString(R.string.arpeggio_head), getString(R.string.arpeggio_body)
        ));
        capsules.add(new Capsule(R.drawable.capriccio, getString(R.string.capriccio_name),
        		Category.Espresso, R.drawable.str05, new int[] { Cup.Espresso },
                getString(R.string.capriccio_head), getString(R.string.capriccio_body)
        ));
        capsules.add(new Capsule(R.drawable.caramelito, getString(R.string.caramelito_name),
        		Category.Variations, R.drawable.str06, new int[] { Cup.Espresso, Cup.Cappuccino },
                getString(R.string.caramelito_head), getString(R.string.caramelito_body)
        ));
        capsules.add(new Capsule(R.drawable.ciocattino, getString(R.string.ciocattino_name),
        		Category.Variations, R.drawable.str06, new int[] { Cup.Espresso, Cup.Cappuccino },
                getString(R.string.ciocattino_head), getString(R.string.ciocattino_body)
        ));
        capsules.add(new Capsule(R.drawable.cosi, getString(R.string.cosi_name),
        		Category.Espresso, R.drawable.str03, new int[] { Cup.Espresso },
                getString(R.string.cosi_head), getString(R.string.cosi_body)
        ));
        capsules.add(new Capsule(R.drawable.crealto, getString(R.string.crealto_name),
        		Category.Espresso, R.drawable.str08, new int[] { Cup.Espresso },
                getString(R.string.crealto_head), getString(R.string.crealto_body)
        ));
        capsules.add(new Capsule(R.drawable.decaffeinato, getString(R.string.decaffeinato_name),
        		Category.Espresso, R.drawable.str02, new int[] { Cup.Espresso },
                getString(R.string.decaffeinato_head), getString(R.string.decaffeinato_body)
        ));
        capsules.add(new Capsule(R.drawable.decaffeinatointenso, getString(R.string.decaffeinato_intenso_name),
        		Category.Espresso, R.drawable.str07, new int[] { Cup.Espresso },
                getString(R.string.decaffeinato_intenso_head), getString(R.string.decaffeinato_intenso_body)
        ));
        capsules.add(new Capsule(R.drawable.decaffeinatolungo, getString(R.string.decaffeinato_lungo_name),
        		Category.Lungo, R.drawable.str03, new int[] { Cup.Lungo },
                getString(R.string.decaffeinato_lungo_head), getString(R.string.decaffeinato_lungo_body)
        ));
        capsules.add(new Capsule(R.drawable.dharkan, getString(R.string.dharkan_name),
        		Category.Intenso, R.drawable.str11, new int[] { Cup.Ristretto, Cup.Espresso, Cup.Cappuccino },
                getString(R.string.dharkan_head), getString(R.string.dharkan_body)
        ));
        capsules.add(new Capsule(R.drawable.dhjana, getString(R.string.dhjana_name),
        		Category.LimitedEdition, R.drawable.str08, new int[] { Cup.Espresso },
                getString(R.string.dhjana_head), getString(R.string.dhjana_body)
        ));
        capsules.add(new Capsule(R.drawable.dulsaodobrasil, getString(R.string.new_dulsao_do_brasil_name),
        		Category.PureOrigin, R.drawable.str04, new int[] { Cup.Espresso },
                getString(R.string.new_dulsao_do_brasil_head), getString(R.string.new_dulsao_do_brasil_body)
        ));
        capsules.add(new Capsule(R.drawable.finezzolungo, getString(R.string.finezzo_lungo_name),
        		Category.Lungo, R.drawable.str03, new int[] { Cup.Lungo },
                getString(R.string.finezzo_lungo_head), getString(R.string.finezzo_lungo_body)
        ));
        capsules.add(new Capsule(R.drawable.fortissiolungo, getString(R.string.fortissio_lungo_name),
        		Category.Lungo, R.drawable.str07, new int[] { Cup.Lungo },
                getString(R.string.fortissio_lungo_head), getString(R.string.fortissio_lungo_body)
        ));
        capsules.add(new Capsule(R.drawable.variantionshazelnut, getString(R.string.variations_hazelnut_name),
                Category.Variations, R.drawable.str06, new int[] { Cup.Espresso },
                getString(R.string.variations_hazelnut_head), getString(R.string.variations_hazelnut_body)
        ));
        capsules.add(new Capsule(R.drawable.indriyafromindia, getString(R.string.indriya_from_india_name),
        		Category.PureOrigin, R.drawable.str10, new int[] { Cup.Espresso },
                getString(R.string.indriya_from_india_head), getString(R.string.indriya_from_india_body)
        ));
        capsules.add(new Capsule(R.drawable.kazaar, getString(R.string.kazaar_name),
        		Category.Intenso, R.drawable.str12, new int[] { Cup.Ristretto },
                getString(R.string.kazaar_head), getString(R.string.kazaar_body)
        ));
        capsules.add(new Capsule(R.drawable.liniziolungo, getString(R.string.linizio_lungo_name),
        		Category.Lungo, R.drawable.str04, new int[] { Cup.Lungo },
                getString(R.string.linizio_lungo_head), getString(R.string.linizio_lungo_body)
        ));
        capsules.add(new Capsule(R.drawable.livanto, getString(R.string.livanto_name),
        		Category.Espresso, R.drawable.str06, new int[] { Cup.Espresso },
                getString(R.string.livanto_head), getString(R.string.livanto_body)
        ));
        capsules.add(new Capsule(R.drawable.variationsmacadamia, getString(R.string.variations_macadamia_name),
                Category.Espresso, R.drawable.str06, new int[] { Cup.Espresso },
                getString(R.string.variations_macadamia_head), getString(R.string.variations_macadamia_body)
        ));
        capsules.add(new Capsule(R.drawable.naora, getString(R.string.naora_name),
        		Category.LimitedEdition, R.drawable.str05, new int[] { Cup.Espresso },
                getString(R.string.naora_head), getString(R.string.naora_body)
        ));
        capsules.add(new Capsule(R.drawable.napoli, getString(R.string.napoli_name),
        		Category.LimitedEdition, R.drawable.str11, new int[] { Cup.Ristretto },
                getString(R.string.napoli_head), getString(R.string.napoli_body)
        ));
        capsules.add(new Capsule(R.drawable.ristretto, getString(R.string.ristretto_name),
        		Category.Intenso, R.drawable.str10, new int[] { Cup.Ristretto, Cup.Espresso, Cup.Cappuccino },
                getString(R.string.ristretto_head), getString(R.string.ristretto_body)
        ));
        capsules.add(new Capsule(R.drawable.roma, getString(R.string.roma_name),
        		Category.Intenso, R.drawable.str08, new int[] { Cup.Ristretto, Cup.Espresso },
                getString(R.string.roma_head), getString(R.string.roma_body)
        ));
        capsules.add(new Capsule(R.drawable.rosabayadecolombia, getString(R.string.rosabaya_de_colombia_name),
        		Category.PureOrigin, R.drawable.str06, new int[] { Cup.Espresso },
                getString(R.string.rosabaya_de_colombia_head), getString(R.string.rosabaya_de_colombia_body)
        ));
        capsules.add(new Capsule(R.drawable.trieste, getString(R.string.trieste_name),
        		Category.LimitedEdition, R.drawable.str09, new int[] { Cup.Ristretto, Cup.Espresso },
                getString(R.string.trieste_head), getString(R.string.trieste_body)
        ));
        capsules.add(new Capsule(R.drawable.vanilio, getString(R.string.vanilio_name),
        		Category.Variations, R.drawable.str06, new int[] { Cup.Espresso, Cup.Cappuccino },
                getString(R.string.vanilio_head), getString(R.string.vanilio_body)
        ));
        capsules.add(new Capsule(R.drawable.vivaltolungo, getString(R.string.vivalto_lungo_name),
        		Category.Lungo, R.drawable.str04, new int[] { Cup.Lungo },
                getString(R.string.vivalto_lungo_head), getString(R.string.vivalto_lungo_body)
        ));
        capsules.add(new Capsule(R.drawable.volluto, getString(R.string.volluto_name),
        		Category.Espresso, R.drawable.str04, new int[] { Cup.Espresso },
                getString(R.string.volluto_head), getString(R.string.volluto_body)
        ));
    }

    private void showCapsuleDialog(Capsule capsule) {
    	if (capsuleCategory == null) {
    		capsuleCategory = getResources().getStringArray(R.array.capsulecategory);
    	}

        final Dialog dialog = new Dialog(this);
        dialog.setContentView(R.layout.capsuledialog);
        dialog.setTitle(capsule.getName());
        dialog.setCancelable(true);
        dialog.setCanceledOnTouchOutside(true);

		((ImageView) dialog.findViewById(R.id.image)).setImageResource(capsule.getImageId());
		((ImageView) dialog.findViewById(R.id.strength)).setImageResource(capsule.getStrengthId());
		((TextView) dialog.findViewById(R.id.name)).setText(capsule.getName());
		((TextView) dialog.findViewById(R.id.teaser)).setText(capsule.getTeaser());        

		TextView tv = (TextView) dialog.findViewById(R.id.description);
		tv.setText(capsule.getDescription());

		int category = capsule.getCategory();
		String s = "";
		if (category >= 1 && category <= capsuleCategory.length) {
			s = capsuleCategory[category];
		}

		((TextView) dialog.findViewById(R.id.category)).setText(s);
		ImageView iv = (ImageView) dialog.findViewById(R.id.tasting);
		iv.setImageBitmap(getTastingBitmap(capsule.getTasting()));
		
		iv.setOnTouchListener(new View.OnTouchListener() {
			public boolean onTouch(View v, MotionEvent event) {
				LayoutInflater inflater = getLayoutInflater();
				View layout = inflater.inflate(R.layout.cupstoast, (ViewGroup) findViewById(R.id.cupstoast));

				Toast toast = new Toast(getApplicationContext());
				toast.setGravity(Gravity.CENTER_VERTICAL, 0, 0);
				toast.setDuration(Toast.LENGTH_LONG);
				toast.setView(layout);
				toast.show();
				return false;
			}
		});

		Button button = (Button) dialog.findViewById(R.id.ok);
        button.setOnClickListener(new OnClickListener() {
        	public void onClick(View v) {
        		dialog.cancel();
            }
        });

        dialog.show();
    }
    
    private Bitmap getTastingBitmap(int[] tasting) {
    	Bitmap[] bms = new Bitmap[tasting.length];
    	int w = -TASTING_KERN, h = 0, x = 0, i;
    	
    	for (i = 0; i < tasting.length; i++) {
    		bms[i] = BitmapFactory.decodeResource(getResources(), cups[tasting[i] - 1]);
    		if (bms[i].getHeight() > h) {
    			h = bms[i].getHeight();
    		}
    		
    		w += TASTING_KERN + bms[i].getWidth();
    	}

        Bitmap cs = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888); 
        Canvas canvas = new Canvas(cs); 

        for (i = 0; i < tasting.length; i++) {
        	canvas.drawBitmap(bms[i], (float) x, (float) (h - bms[i].getHeight()), null);
        	x += bms[i].getWidth() + TASTING_KERN;
        }
        
        return cs;
    }
}