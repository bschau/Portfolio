//
//  ViewController.swift
//  BMICalculator
//
//  Created by Brian Schau on 30/06/2023.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var myResult: UILabel!
      
    @IBOutlet weak var myHeight: UITextField!
    @IBOutlet weak var myWeight: UITextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
    }


    @IBAction func btnCalculate(_ sender: Any) {
        var h: Double = 0
        if let hText = myHeight.text {
            if let hDouble = Double(hText) {
                h = hDouble / 100
            }
        }
        
        if Int(h) == 0 {
            myResult.text = "You must be at least 100 cm."
            return
        }
        
        var w: Double = 0
        if let wText = myWeight.text {
            if let wDouble = Double(wText) {
                w = wDouble
            }
        }
                
        let bmi = w / (h * h)
        
        let classification = getClassification(bmi: bmi)
        
        let formattedBMI = String(format: "%.1f", bmi)
        
        myResult.text = "BMI: \(formattedBMI), \(classification)"
    }
    
    func getClassification(bmi: Double) -> String {
        if bmi < 18.5 {
            return "Underweight"
        } else if bmi < 24.9 {
            return "Healthy weight"
        } else if bmi < 29.9 {
            return "Overweight"
        }
        
        return "Obese"
    }
}

